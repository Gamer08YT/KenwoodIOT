//
// Created by Jan Heil on 02.07.2023.
//

#include "HomeAssistant.h"

// Webserver and WebSerial
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoHA.h>
#include <WiFiClient.h>
#include "main.cpp"

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
 * Prepare HomeAssistant.
 */
void prepare() {
    connectWiFi();
    connectMQTT();
    prepareComponents();
}

void connectWiFi() {
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
}

void connectMQTT() {
    // Add MQTT Listener.
    mqtt.onMessage(onMessage);
    mqtt.onConnected(onConnected);

    // Connect to HomeAssistant.
    mqtt.begin("homeassistant.local", "energyswitch", "energyswitch");
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

void prepareComponents() {
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
}