//
// Created by Jan Heil on 23.07.2023.
//

#include <cstdint>
#include <pins_arduino.h>
#include <Arduino.h>
#include "Remote.h"
#include "Device.h"
#include <IRremote.hpp>

// Receive Pin: (D3 => 0)
const uint8_t PIN_REC = D3;

// Send Pin: (D4 => 02)
const uint8_t PIN_SEND = D4;

// Store Address of IR Frame.
const uint8_t addressIO = 0xB8;

/**
 * Send Command via IR LED.
 * @param dataIO
 */
void Remote::send_cmd(int dataIO) {
    // Write in NEC Format.
    IrSender.sendNEC(addressIO, dataIO, 1);

    // Print Debug Message.
    Device::print("Send IR Command ");
    Device::println(String(dataIO, HEX));
}

/**
 * Prepare PINS for IR SEND/RECEIVE.
 */
void Remote::prepare() {
    // Setup Pin Modes.
    pinMode(PIN_REC, INPUT);
    pinMode(PIN_SEND, OUTPUT);

    // Begin IR Sender.
    IrSender.begin(PIN_SEND);

    // Begin IR Receiver.
    IrReceiver.begin(PIN_REC, LED_BUILTIN);

    // Print Debug Message.
    Device::println("Registered IR Sender and Receiver.");
}

/**
 * Handle Loop Function for IR.
 */
void Remote::handleIR() {
    if (IrReceiver.decode()) {
        // Handle IR Data...


        // Resume IR Frame Buffer.
        IrReceiver.resume();
    }
}
