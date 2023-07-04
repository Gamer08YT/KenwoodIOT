#include <Arduino.h>

// Webserver and WebSerial
#include <ESP8266WiFi.h>
#include <ArduinoHA.h>
#include <WiFiClient.h>
#include <HomeAssistant.h>
#include <MQTT.h>

// Device Classes
#include "Kenwood.h"
#include "Device.h"


/*
    General commands:
    10 00: switch on (from standby)
    10 80: switch off (to standby)

    AMP commands (KRF-A4020):
    18 48: activate MD input
    08 48: activate CD input
    04 49: activate Tape input
    F8 48: activate Phono input

    AMP Response:
    80 x5: Input selected (0=phono, C=aux, 8=tuner, 4=cd,
       B=md, A=Tape1)
 */

// Define Network Credentials
const char *ssid = "HRouter-1";
const char *password = "13280073116925758533";

// Define WiFi Client.
WiFiClient client;

// Define Home Assistant Credentials.
HADevice device;

// Define Home Assistant MQTT Instance.
HAMqtt mqtt(client, device);

// Store Volume Slider.
HANumber volume("Volume");

// Store Bus Version Select.
HASelect version("type");

// Store Input Select.
HASelect input("Input");

// Store Mute Switch.
HASwitch mute("Mute");

// Store Standby Switch.
HASwitch standby("Standby");

// Store Power Switch.
HASwitch power("Power");

/**
 * Bread Board:
 * -----------------
 * |       |       |
 * | DATA  | BUSY  |
 * |       |       |
 * -----------------
 */

/*
Kenwood XS/XS8/SL16 system remote control
pinout: sleeve = GND ring = PIN_DATA tip = PIN_BUSY

           R
pin3 o---[   ]----o tip
          330

           R
pin2 o---[   ]----o ring
          330

for Kenwood KXF-W4010

CMD	        XS8	            SL16
-----------------------------------------------------
ff B 	 0100 0010 (0x42) 11111010 0010 0100 (0xFA24)
ff A 	 0100 0011 (0x43) 11111010 0011 0100 (0xFA34)
stop B 	 0100 0100 (0x44) 11111010 0100 0100 (0xFA44)
stop A 	 0100 0101 (0x45) 11111010 0101 0100 (0xFA54)
> B 	 0100 0110 (0x46) 11111010 0110 0100 (0xFA64)
> A 	 0100 0111 (0x47) 11111010 0111 0100 (0xFA74)
rec B 	 0100 1000 (0x48) 11111010 1000 0100 (0xFA84)
rew B 	 0100 1010 (0x4A) 11111010 1010 0100 (0xFAA4)
rew A 	 0100 1011 (0x4B) 11111010 1011 0100 (0xFAB4)
pause B  0100 1100 (0x4C) 11111010 1100 0100 (0xFAC4)
< B 	 0100 1110 (0x4E) 11111010 1110 0100 (0xFAE4)
< A 	 0100 1111 (0x4F) 11111010 1111 0100 (0xFAF4)
*/

//arduino pins

class beginWebserver;

void receiveMessage(uint8_t *data, size_t length);

void try_all(int wait);

void send_cmd(uint16_t cmd);

void handleCommand(int valueIO);

void print(String contentIO, int filterIO = 0);

void println(String contentIO, int filterIO = 0);

void print_info();

void print_device();

void setPins();

void onConnected();

void onMessage(const char *topic, const uint8_t *payload, uint16_t length);

void setup() {
    // Begin Serial.
    Serial.begin(115200);
    Serial.setDebugOutput(true);

    prepare();

    // Connect to Wi-Fi Network.
    WiFi.begin(ssid, password);

    // Wait until Device is Connected.
    while (WiFi.status() != WL_CONNECTED) {
        // Print Waiting Point.
        Serial.print(".");

        // Let Status LED Blink.
        Device::status_led(150);
    }

    // Begin OTA Server.
    Device::beginOTA();

    // Start WebServer
    Device::beginWebserver();

    // Register all Commands.
    Device::addCommands();

    // Start Telnet Stream.
    Device::beginTelnet();

    // Prepare OTA for Remote Programming.
    //ArduinoOTA.setHostname("energyswitch");
    //ArduinoOTA.setPassword("ByteSwitch");

    // Print Success Message.
    Serial.println("Connected to WiFi.");

    // Get MAC Address into Buffer.
    byte mac[WL_MAC_ADDR_LENGTH];
    WiFi.macAddress(mac);

    // Set ID of Device.
    device.setUniqueId(mac, sizeof(mac));

    // Prepare for Home Assistant.
    device.setName("Energy Switch");
    device.setSoftwareVersion("1.0.0");
    device.setModel("ESP8266");
    device.setManufacturer("Jan Heil (www.byte-store.de)");
    device.enableSharedAvailability();
    device.enableLastWill();

    // Prepare Volume Slider.
    volume.setMax(100);
    volume.setMin(0);
    volume.setState(0);
    volume.setName("Volume");
    volume.setIcon("mdi:volume-medium");
    volume.setMode(HANumber::ModeSlider);
    volume.onCommand(MQTT::onVolume);

    // Prepare Mute Switch.
    mute.setName("Mute");
    mute.setIcon("mdi:volume-off");
    mute.onCommand(MQTT::onMute);

    // Prepare Standby Switch.
    standby.setName("Standby");
    standby.setIcon("mdi:power-sleep");
    standby.onCommand(MQTT::onStandby);

    // Prepare Power Switch.
    power.setName("Power");
    power.setIcon("mdi:power");
    power.onCommand(MQTT::onPower);

    // Prepare BUS Version.
    version.setName("Typ");
    version.setIcon("mdi:knob");
    version.setOptions("XS8;SL16");
    version.onCommand(MQTT::onVersion);
    version.setCurrentState((Kenwood::getInterface() == 8 ? 0 : 1));


    // Prepare Input Select.
    input.setName("Input");
    input.setIcon("mdi:cable");
    input.setOptions("TV;Phono");
    input.setCurrentState(0);
    input.onCommand(MQTT::onInput);

    // Add MQTT Listener.
    mqtt.onMessage(onMessage);
    mqtt.onConnected(onConnected);

    // Connect to HomeAssistant.
    mqtt.begin("homeassistant.local", "energyswitch", "energyswitch");

    // Set LED Pin.
    setPins();

    // Set Input and Output Pins.
    Kenwood::prepare();

    // Print Info Commands.
    print_info();
}

void onConnected() {
    println("Connected to MQTT Server.");
}

void onMessage(const char *topic, const uint8_t *payload, uint16_t length) {
    print("Received ");
    print(topic);
    println(" - ");
    //(payload);
}

/**
 * Set Default Pins of Device.
 */
void setPins() {
    // Set Status LED as Output.
    pinMode(LED_BUILTIN, OUTPUT);

    // Disable LED.
    digitalWrite(LED_BUILTIN, HIGH);
}

/**
 * Function to Handle WebSerial Messages.
 * @param data
 * @param len
 */
void receiveMessage(uint8_t *data, size_t length) {
    // Create new String Buffer.
    String buffer = "";

    // Appends Chars to Buffer.
    for (int i = 0; i < length; ++i) {
        buffer += char(data[i]);
    }

    // Parse Buffer to Integer.
    long valueIO = buffer.toInt();

    // Print Buffer to Console.
    print("Received Data: ");
    println(buffer);

    // Handle cast Value.
    handleCommand(valueIO);
}


/**
 * Print Message to Serial and WebSerial.
 * @param contentIO
 */
void println(String contentIO, int filterIO) {
    Serial.println(contentIO);
    //WebSerial.println(contentIO);
}

void print(String contentIO, int filterIO) {
    Serial.print(contentIO);
    //WebSerial.print(contentIO);
}

void print_info() {
    // Print Device Usage.
    println("Type:");
    println("  value -8 to set interface to XS/XS8. Default is XS/XS8");
    println("  value -17 shows this Page");
    println("  value -18 show Pin/Device Status");
    println("  value -16 to set interface to SL16");
    println("  value >0 to send command. It will be saved as 'last command'");
    println("  value <-16 to start a loop to automatically try all commands with delay of |value| ms beginning at 'last command'. Default 'last command' = 1");
}

void loop() {
    // Check if Serial is available.
    /*while (Serial.available()) {
        long valueIO = Serial.parseInt();

        // Handle parsed Value.
        handleCommand(valueIO);

        // Loop MQTT.
        mqtt.loop();
    }*/

    // Handle WebServer Client/s.
    Device::handleWebserver();

    // Handle Telnet Input Stream.
    Device::handleTelnet();

    // Handle Serial for Telnet.
    if (Serial.available()) {
        Device::getTelnet().print(Serial.read());
    }

    // Handle MQTT Stream.
    mqtt.loop();
}