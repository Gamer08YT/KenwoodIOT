#include <Arduino.h>

// Webserver and WebSerial
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>
#include <ArduinoHA.h>
#include <WiFiClient.h>

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

// Store Input Select.
HASelect input("Input");

// Store Mute Switch.
HASwitch mute("Mute");

// Store Standby Switch.
HASwitch standby("Standby");

// Store Power Switch.
HASwitch power("Power");

// Store Bus Version.
HASelect version("Version");

/**
 * Bread Board:
 * -----------------
 * |       |       |
 * | DATA  | BUSY  |
 * |       |       |
 * -----------------
 */

// Define new Server Object.
AsyncWebServer server(80);

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
const uint8_t PIN_DATA = D2;
const uint8_t PIN_BUSY = D1;
const uint8_t RESPONSE = 4;//for loop stopping at event like motor start

uint16_t last_cmd = 1;
uint8_t interface = 16;

//timing XS8
const uint8_t START_BIT_L_8 = 1;
const uint8_t START_BIT_H_8 = 10;
const uint8_t BIT_0L_8 = 10;
const uint8_t BIT_1L_8 = 5;
const uint8_t BIT_H_8 = 5;

//timing SL16
const uint8_t START_BIT_L_16 = 5;
const uint8_t START_BIT_H_16 = 5;
const uint8_t BIT_0L_16 = 5;
const uint8_t BIT_1L_16 = 2;
const uint8_t BIT_H_16 = 2;

//delays
uint8_t START_BIT_L;
uint8_t START_BIT_H;
uint8_t BIT_0L;
uint8_t BIT_1L;
uint8_t BIT_H;

void set_interface(int i);

void receiveMessage(uint8_t *data, size_t length);

void try_all(int wait);

void send_cmd(uint16_t cmd);

void handleCommand(int valueIO);

void print(String contentIO, int filterIO = 0);

void println(String contentIO, int filterIO = 0);

void status_led(int durationIO);

void print_info();

void print_device();

void setPins();

void onConnected();

void onMessage(const char *topic, const uint8_t *payload, uint16_t length);

void setup() {
    // Begin Serial.
    Serial.begin(115200);
    Serial.setDebugOutput(true);

    // Connect to Wi-Fi Network.
    WiFi.begin(ssid, password);

    // Wait until Device is Connected.
    while (WiFi.status() != WL_CONNECTED) {
        // Print Waiting Point.
        Serial.print(".");

        // Let Status LED Blink.
        status_led(150);
    }

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
    volume.setName("Volume");
    volume.setIcon("mdi:volume-medium");
    volume.setMode(HANumber::ModeSlider);

    // Prepare Mute Switch.
    mute.setName("Mute");
    mute.setIcon("mdi:volume-off");

    // Prepare Standby Switch.
    standby.setName("Standby");
    standby.setIcon("mdi:power-sleep");

    // Prepare Power Switch.
    power.setName("Power");
    power.setIcon("mdi:power");

    // Prepare Input Select.
    input.setName("Input");
    input.setIcon("mdi:knob");
    input.setOptions("TV;Phono");

    // Prepare BUS Version.
    version.setIcon("mdi:cable-data");
    version.setName("BUS-Version");
    version.setOptions("XS8;SL16");
    version.setCurrentState((interface == 8 ? 0 : 1));

    // Add MQTT Listener.
    mqtt.onMessage(onMessage);
    mqtt.onConnected(onConnected);

    // Connect to HomeAssistant.
    mqtt.begin("homeassistant.local", "energyswitch", "energyswitch");

    // Set Input and Output Pins.
    //setPins();

    // Set Default Interface Version.
    //set_interface(-16);

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
    //pinMode(RESPONSE, INPUT);
    pinMode(PIN_BUSY, INPUT);
    pinMode(PIN_DATA, INPUT);

    // Set Status LED as Output.
    pinMode(LED_BUILTIN, OUTPUT);

    // Disable LED.
    digitalWrite(LED_BUILTIN, LOW);
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
 * Set Bus Interface Version.
 * @param i
 */
void set_interface(int i) {
    interface = -i;
    print("Set interface to ");
    println(String(interface), DEC);
    println(String(interface), DEC);

    if (interface == 8) {
        START_BIT_L = START_BIT_L_8;
        START_BIT_H = START_BIT_H_8;
        BIT_0L = BIT_0L_8;
        BIT_1L = BIT_1L_8;
        BIT_H = BIT_H_8;
    } else {
        START_BIT_L = START_BIT_L_16;
        START_BIT_H = START_BIT_H_16;
        BIT_0L = BIT_0L_16;
        BIT_1L = BIT_1L_16;
        BIT_H = BIT_H_16;
    }
}

/**
 * Send CMD to Receiver.
 * @param cmd
 */
void send_cmd(uint16_t cmd) {
    print("Interface ");
    print(String(interface), DEC);
    print(" Command ");
    print(String(cmd), BIN);
    print(" / 0x");
    println(String(cmd), HEX);

    // Check if Device is Busy.
    if (!digitalRead(PIN_BUSY)) {
        // Set LED Status for Write Mode.
        digitalWrite(LED_BUILTIN, HIGH);

        pinMode(PIN_BUSY, OUTPUT);
        pinMode(PIN_DATA, OUTPUT);
        digitalWrite(PIN_BUSY, LOW);
        digitalWrite(PIN_DATA, LOW);
        //PIN_BUSY on
        digitalWrite(PIN_BUSY, HIGH);
        //start bit
        delay(START_BIT_L);
        digitalWrite(PIN_DATA, HIGH);
        delay(START_BIT_H);

        //PIN_DATA bits
        for (uint16_t mask = 1U << (interface - 1); mask; mask >>= 1) {
            digitalWrite(PIN_DATA, LOW);
            if (cmd & mask)
                delay(BIT_1L);
            else
                delay(BIT_0L);
            digitalWrite(PIN_DATA, HIGH);
            delay(BIT_H);
        }

        //end command
        digitalWrite(PIN_DATA, LOW);
        //PIN_BUSY off
        digitalWrite(PIN_BUSY, LOW);
        delay(1);
        //go standby
        pinMode(PIN_BUSY, INPUT);
        pinMode(PIN_DATA, INPUT);
        last_cmd = cmd;

        // Set LED Status for Write Mode.
        digitalWrite(LED_BUILTIN, LOW);

        // Print Debug Message.
        println("Sent Command to Kenwood");
    } else {
        // Let LED Blink 2 Times.
        status_led(150);
        status_led(150);

        // Print Busy Message.
        println("Device is Busy.");
    }
}

/**
 * Let Status LED Blink for Duration.
 * @param durationIO
 */
void status_led(int durationIO) {
    // Set LED Status for Write Mode.
    digitalWrite(LED_BUILTIN, HIGH);

    // Wait duration.
    delay(durationIO);

    // Set LED Status for Write Mode.
    digitalWrite(LED_BUILTIN, LOW);

    // Wait duration.
    delay(durationIO);
}

/**
 * Try all stored Commands.
 * @param wait
 */
void try_all(int wait) {
    println("Trying all Commands");

    for (uint16_t cmd = last_cmd; cmd < 1U << (interface - 1); cmd++) {
        send_cmd(cmd);
        delay(-wait);
        if (digitalRead(RESPONSE))
            break;
    }
}

/**
 * Handle given Integer as Command.
 * @param valueIO
 */
void handleCommand(int valueIO) {
    // Check for XS8/SL16 Commands.
    if (valueIO == -8 || valueIO == -16) {
        set_interface(valueIO);
    } else if (valueIO == -17) {
        print_info();
    } else if (valueIO == -18) {
        print_device();
    } else if (valueIO > 0) {
        send_cmd(valueIO);
    } else if (valueIO < -16) {
        try_all(valueIO);
    }
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

void print_device() {
    // Set Busy Pin to Input.
    pinMode(PIN_BUSY, INPUT);

    // Set Data Pin to Input.
    pinMode(PIN_DATA, INPUT);

    // Print Pin Info.
    println("Pins:");
    print("Busy: ");
    println(String(digitalRead(PIN_BUSY)));
    print("Data: ");
    println(String(digitalRead(PIN_DATA)));

    // Print Device Info.
    println("");
    println("Device:");
    print("MAC: ");
    println(WiFi.macAddress());
    print("IP-Address: ");
    println(WiFi.localIP().toString());

    // Print MQTT Info.
    println("");
    println("MQTT:\n");
    print("Connected: ");
    println(String(mqtt.isConnected()));

    // Print HomeAssistant Info.
    println("");
    println("HomeAssistant:\n");
    print("Available: ");
    println(String(device.isAvailable()));
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

    mqtt.loop();
}