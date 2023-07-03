//
// Created by Jan Heil on 02.07.2023.
//

#include <Arduino.h>
#include "Device.h"
#include "EscapeCodes.h"
#include "commands/Command.h"
#include "commands/InfoCommand.h"


// Store Telnet Instance.
ESPTelnet telnet;
EscapeCodes ansi;

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
    telnet.println("\nWelcome " + telnet.getIP());
    telnet.println("(Use ^] + q  to disconnect.)");
    telnet.print(ansi.reset());
    telnet.print("\n\n> ");
}

/**
 * On Telnet Data Input.
 * @param dataIO
 */
void Device::onInput(String dataIO) {
    Serial.println(dataIO);

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
    Device::println(commandIO->invoke());

    // Push Command into Vector.
    commands.push_back(commandIO);
}

/**
 * Register all Hard-Coded Commands.
 */
void Device::addCommands() {
    Device::addCommand(new InfoCommand());
}






