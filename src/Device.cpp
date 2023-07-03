//
// Created by Jan Heil on 02.07.2023.
//

#include <Arduino.h>
#include "Device.h"
#include "TelnetPrint.h"

/**
 * Let Status LED Blink for Duration.
 * @param durationIO
 */
void Device::status_led(int durationIO) {
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
 * Begin Telnet Stream Server.
 */
void Device::beginTelnet() {
    TelnetPrint.begin();
}

/**
 * Println Message to Serial and Telnet.
 * @param dataIO
 */
void Device::println(String dataIO) {
    Serial.println(dataIO);
    TelnetPrint.println(dataIO);
}

/**
 * Print Message to Serial and Telnet.
 * @param dataIO
 */
void Device::print(String dataIO) {
    Serial.print(dataIO);
    TelnetPrint.print(dataIO);
}

/**
 * Write Message to Serial and Telnet.
 * @param dataIO
 */
void Device::write(String dataIO) {
    Serial.write(dataIO.toInt());
    TelnetPrint.write(dataIO.toInt());
}

/**
 * Handle Telnet Input Stream.
 */
void Device::handleTelnet() {
    // Get TClient from Telnet Class.
    NetClient clientIO = TelnetPrint.available();

    // Check if Client is set.
    if (clientIO) {
        // Read Char from Stream.
        String charIO = String(clientIO.read());

        // Handle Close Command.
        if (charIO == "C") {
            // Print Bye Message and Stop Socket Stream.
            clientIO.println("bye bye");
            clientIO.flush();
            clientIO.stop();
        } else {
            Device::write(charIO);
        }
    }
}




