//
// Created by Jan Heil on 02.07.2023.
//

#include <Arduino.h>
#include "Device.h"
#include "commands/Command.h"
#include "commands/InfoCommand.h"
#include "commands/HelpCommand.h"
#include "commands/ByeCommand.h"
#include "Kenwood.h"
#include "commands/ResetCommand.h"
#include "commands/SendCommand.h"
#include "commands/ClearCommand.h"
#include "ElegantOTA.h"


// Store Telnet Instance.
ESPTelnet telnet;
EscapeCodes ansi;

// Store WebServer Instance.
ESP8266WebServer server(80);

// Store Commands.
std::vector<Command *> commands;

/**
 * Let Status LED Blink for Duration.
 * @param durationIO
 */
void Device::status_led(int durationIO) {
    // Set LED Status for Write Mode.
    digitalWrite(LED_BUILTIN, LOW);

    // Wait duration.
    delay(durationIO);

    // Set LED Status for Write Mode.
    digitalWrite(LED_BUILTIN, HIGH);

    // Wait duration.
    delay(durationIO);
}

/**
 * Begin Telnet Stream Server.
 */
void Device::beginTelnet() {
    // Add Telnet Listener.
    telnet.onConnect(Device::onConnect);
    telnet.onDisconnect(Device::onDisconnect);
    telnet.onInputReceived(Device::onInput);

    // Begin Telnet Server.
    telnet.begin();
}

/**
 * Println Message to Serial and Telnet.
 * @param dataIO
 */
void Device::println(String dataIO) {
    Serial.println(dataIO);
    telnet.println(dataIO);
}

/**
 * Print Message to Serial and Telnet.
 * @param dataIO
 */
void Device::print(String dataIO) {
    Serial.print(dataIO);
    telnet.print(dataIO);
}

/**
 * Write Message to Serial and Telnet.
 * @param dataIO
 */
void Device::write(String dataIO) {
    Serial.write(dataIO.toInt());
    telnet.print(dataIO.toInt());
}

/**
 * Handle Telnet Input Stream.
 */
void Device::handleTelnet() {
    // Handle Telnet Loop Function.
    telnet.loop();
}

/**
 * On Telnet Client Connect.
 * @param ipIO
 */
void Device::onConnect(String ipIO) {
    Serial.println("New Client Connected.");

    // Print Welcome Message to Client.
    telnet.print(ansi.cls());
    telnet.print(ansi.home());
    telnet.print(ansi.setFG(ANSI_BRIGHT_WHITE));
    telnet.print("\nWelcome " + telnet.getIP());
    telnet.print(" found ");
    telnet.print(commands.size());
    telnet.print((commands.size() == 1 ? " Command" : " Commands"));
    telnet.println(" :-)");
    telnet.println("(Use ^] + q  to disconnect.)");
    telnet.print(ansi.reset());
    telnet.print("\n\n> ");

    Command *command = new InfoCommand();
    Serial.println(command->invoke());
    delete command;
}

/**
 * On Telnet Data Input.
 * @param dataIO
 */
void Device::onInput(String dataIO) {
    Serial.println(dataIO);

    // Handle Command Input.
    Device::handleCommand(dataIO);

    // Print Shell Line.
    telnet.print("> ");
}

/**
 * On Telnet Client Disconnect.
 * @param addressIO
 */
void Device::onDisconnect(String addressIO) {
    Serial.println("Client has Disconnected.");
}

/**
 * Return Telnet Instance.
 * @return
 */
ESPTelnet Device::getTelnet() {
    return telnet;
}

/**
 * Return ANSI Codes Instance.
 * @return
 */
EscapeCodes Device::getANSI() {
    return ansi;
}

/**
 * Return registered Commands.
 * @return
 */
std::vector<Command *> Device::getCommands() {
    return commands;
}

/**
 * Add Command to Vector.
 * @param commandIO
 */
void Device::addCommand(Command *commandIO) {
    // Print Debug Message.
    Device::print("Added Command ");
    Serial.println(commandIO->invoke());
    Device::println(" to Device.");

    // Push Command into Vector.
    commands.push_back(commandIO);
}

/**
 * Register all Hard-Coded Commands.
 */
void Device::addCommands() {
    Device::addCommand(new HelpCommand());
    Device::addCommand(new InfoCommand());
    Device::addCommand(new ByeCommand());
    Device::addCommand(new ResetCommand());
    Device::addCommand(new SendCommand());
    Device::addCommand(new ClearCommand());
}

/**
 * Handle Command Input.
 * @param dataIO
 */
void Device::handleCommand(String dataIO) {
    // Store Found State.
    bool foundIO = false;

    if (dataIO.startsWith("/")) {
        // Replace Slash Invoke.
        dataIO.replace("/", "");

        // Split via Spaces.
        std::vector<String> splitIO = Device::split(dataIO, " ");

        // Loop trough Commands and Check or Execute.
        for (Command *commandIO: Device::getCommands()) {
            // Check if Command exits/equals.
            if (String(splitIO[0]).equalsIgnoreCase(commandIO->invoke())) {

                // Delete first Vector Item.
                if (!splitIO.empty())
                    splitIO.erase(splitIO.begin());

                // Print Debug Message.
                Device::print("Args: ");
                Device::println(String(splitIO.size()));

                // Execute Commands.
                commandIO->execute(splitIO);

                // Override Found State.
                foundIO = true;
            }
        }

        // Print Error Message.
        if (!foundIO) {
            Device::print("Can't find Command use /help to Display all Commands.");
            Device::println(dataIO);
        }
    }
}

void Device::print_device() {
    // Set Busy Pin to Input.
    pinMode(Kenwood::getBusy(), INPUT);

    // Set Data Pin to Input.
    pinMode(Kenwood::getData(), INPUT);

    // Print Pin Info.
    Device::println("Pins:");
    Device::print("Busy: ");
    Device::println(String(digitalRead(Kenwood::getBusy())));
    Device::print("Data: ");
    Device::println(String(digitalRead(Kenwood::getData())));

    // Print Device Info.
    Device::println("\nDevice:");
    Device::print("MAC: ");
    Device::println(WiFi.macAddress());
    Device::print("IP-Address: ");
    Device::println(WiFi.localIP().toString());
    Device::print("VCC: ");
    Device::println(String((float) (ESP.getVcc() / 1024.0f)));
    Device::print("Chip-ID: ");
    Device::println(String(ESP.getChipId()));

    // Print MQTT Info.
    Device::println("\nMQTT:");
    Device::print("Connected: ");
    //Device::println(String(mqtt.isConnected()));

    // Print HomeAssistant Info.
    Device::println("HomeAssistant:\n");
    Device::print("Available: ");
    //Device::println(String(device.isAvailable()));
}


/**
 * Split String by delimiter.
 * @param valueIO
 * @param delimiterIO
 * @return
 */
std::vector<String> Device::split(String &valueIO, const char *delimiterIO) {
    std::vector<String> returnIO;
    int indexIO = 0;
    while (true) {
        int positionIO = valueIO.indexOf(delimiterIO, indexIO);
        if (positionIO == -1) {
            returnIO.push_back(valueIO.substring(indexIO));
            break;
        } else {
            returnIO.push_back(valueIO.substring(indexIO, positionIO));
            indexIO = positionIO + strlen(delimiterIO);
        }
    }

    return returnIO;
}

/**
 * Begin OTA (Over the Air) Server.
 */
void Device::beginOTA() {
    // Begin Elegant OTA via Web Server.
    ElegantOTA.begin(&server, "ByteSwitch", "ByteSwitch");
}

/**
 * Begin Web Server.
 */
void Device::beginWebserver() {
    // Send Default Index Message.
    server.on("/", []() {
        server.send(200, "text/html", "<h1>EnergySwitch</h1><h3>Made with Love by Jan Heil (www.byte-store.de)</h3>");
    });

    // Begin Server.
    server.begin();
}

/**
 * Handle WebServer Client/s.
 */
void Device::handleWebserver() {
    server.handleClient();
}







