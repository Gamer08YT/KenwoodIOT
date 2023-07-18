//
// Created by Jan Heil on 02.07.2023.
//


#include <HardwareSerial.h>
#include <Arduino.h>
#include "Kenwood.h"
#include "Device.h"

//Data Pin: (D2 => 04)
const uint8_t PIN_DATA = 04;

// Busy Pin: (D1 => 05)
const uint8_t PIN_BUSY = 05;

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


/**
 * Toggle Standby State.
 * @param state
 */
void Kenwood::standby(bool state) {
    if (state)
        Kenwood::send_cmd(0x1000);
    else
        Kenwood::send_cmd(0x1080);
}

/**
 * Toggle Input of AMP.
 * @param state
 * eq. CD;Phono;
 */
void Kenwood::setInput(int state) {
    switch (state) {
        case 0:
            Kenwood::send_cmd(0x0848);
            break;
        case 1:
            Kenwood::send_cmd(0xF848);
            break;
        default:
            Serial.println("Can't find Input Address.");
            break;
    }
}

/**
 * Handle given Integer as Command.
 * @param valueIO
 */
void Kenwood::handleCommand(int valueIO) {
    // Check for XS8/SL16 Commands.
    if (valueIO == -8 || valueIO == -16) {
        Kenwood::set_interface(valueIO);
    } else if (valueIO == -17) {
        //print_info();
    } else if (valueIO == -18) {
        //print_device();
    } else if (valueIO > 0) {
        Kenwood::send_cmd(valueIO);
    } else if (valueIO < -16) {
        Kenwood::try_all(valueIO);
    }
}

/**
 * Set Bus Interface Version.
 * @param i
 */
void Kenwood::set_interface(int i) {
    interface = -i;
    Serial.print("Set interface to ");
    Serial.println(interface, DEC);
    Serial.println(interface, DEC);

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
 * Try all stored Commands.
 * @param wait
 */
void Kenwood::try_all(int wait) {
    Serial.println("Trying all Commands");

    for (uint16_t cmd = last_cmd; cmd < 1U << (interface - 1); cmd++) {
        send_cmd(cmd);
        delay(-wait);
        if (digitalRead(RESPONSE))
            break;
    }
}


/**
 * Send CMD to Receiver.
 * @param cmd
 */
void Kenwood::send_cmd(uint16_t cmd) {
    Device::print("Interface ");
    Device::print(String(interface, DEC));
    Device::print(" Command ");
    Device::print(String(cmd, BIN));
    Device::print(" / 0x");
    Device::println(String(cmd, HEX));

    // Check if Device is Busy.
    if (!digitalRead(PIN_BUSY)) {
        pinMode(PIN_BUSY, OUTPUT);
        pinMode(PIN_DATA, OUTPUT);
        Serial.begin(115200);
        digitalWrite(PIN_BUSY, LOW);
        digitalWrite(PIN_DATA, LOW);

        // Set Busy Lock.
        digitalWrite(PIN_BUSY, HIGH);

        // Set LED Status for Write Mode.
        digitalWrite(LED_BUILTIN, HIGH);

        // Send Start Bit with Delay.
        delay(START_BIT_L);
        digitalWrite(PIN_DATA, HIGH);

        // Set LED Status for Write Mode.
        digitalWrite(LED_BUILTIN, LOW);

        // Delay next Bit.
        delay(START_BIT_H);

        //PIN_DATA bits
        for (uint16_t mask = 1U << (interface - 1); mask; mask >>= 1) {
            digitalWrite(PIN_DATA, LOW);

            // Set LED Status for Write Mode.
            digitalWrite(LED_BUILTIN, HIGH);

            if (cmd & mask)
                delay(BIT_1L);
            else
                delay(BIT_0L);
            digitalWrite(PIN_DATA, HIGH);

            // Set LED Status for Write Mode.
            digitalWrite(LED_BUILTIN, LOW);
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
        Serial.begin(115200);
        last_cmd = cmd;

        // Set LED Status for Write Mode.
        digitalWrite(LED_BUILTIN, HIGH);

        // Print Debug Message.
        Device::println("Sent Command to Kenwood");
    } else {
        // Let LED Blink 2 Times.
        Device::status_led(150);
        Device::status_led(150);

        // Print Busy Message.
        Device::println("Device is Busy.");
    }
}

/**
 * Prepare Device.
 */
void Kenwood::prepare() {
    //pinMode(RESPONSE, INPUT);
    pinMode(PIN_BUSY, INPUT_PULLUP);
    pinMode(PIN_DATA, INPUT_PULLUP);

    Serial.begin(115200);

    // Set Default Interface Version.
    //set_interface(-16);
}

int Kenwood::getInterface() {
    return interface;
}

uint8_t Kenwood::getData() {
    return PIN_DATA;
}

uint8_t Kenwood::getBusy() {
    return PIN_BUSY;
}


