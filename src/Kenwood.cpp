//
// Created by Jan Heil on 02.07.2023.
//


#include <HardwareSerial.h>
#include <Arduino.h>
#include "Kenwood.h"
#include "Device.h"
#include "Remote.h"

const unsigned long MSB = 1l << 15; // 16 bits

enum {
    SDAT = 2,
    CTRL = 3,
    BIT_ONE_DELAY_MICROSEC = 3200,
    BIT_TERMINATOR_DELAY_MICROSEC = 2250,
    BIT_GAP_DELAY_MICROSEC = 2250,
    ENABLE_OPEN_COLLECTOR = 1,
};

//Data Pin: (D2 => 04)
const uint8_t PIN_DATA = D5;

// Busy Pin: (D1 => 05)
const uint8_t PIN_BUSY = D6;

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

    // Or send via IR.
    Remote::send_cmd(0x9D);
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

void Kenwood::send_cmd(unsigned long cmd) {
    // Check if Device is Busy.
    if (!digitalRead(PIN_BUSY)) {
        Device::print("Interface ");
        Device::print(String(interface, DEC));
        Device::print(" Command ");
        Device::print(String(cmd, BIN));
        Device::print(" / 0x");
        Device::println(String(cmd, HEX));

        pinMode(getBusy(), OUTPUT);
        pinMode(getData(), OUTPUT);
        Serial.print("Command ");
        Serial.print(cmd, DEC);
        Serial.print(" / 0x");
        Serial.println(cmd, HEX);

        digitalWrite(getData(), LOW);
        if (ENABLE_OPEN_COLLECTOR) {
            pinMode(getData(), INPUT);
        }
        digitalWrite(getBusy(), HIGH);
        delayMicroseconds(5000);

        Kenwood::sendWord(cmd);

        // Return to default state
        digitalWrite(getData(), LOW);
        delayMicroseconds(2000);
        digitalWrite(getBusy(), LOW);

        // Reset PIN Mode.
        Kenwood::prepare();
    } else {
        // Let LED Blink 2 Times.
        Device::status_led(150);
        Device::status_led(150);

        // Print Busy Message.
        Device::println("Device is Busy.");
    }

}

void Kenwood::sendWord(unsigned long word) {
    // StartBit
    if (ENABLE_OPEN_COLLECTOR) {
        pinMode(SDAT, OUTPUT);
    }

    digitalWrite(getData(), HIGH);
    delayMicroseconds(5000);

    for (unsigned long mask = MSB; mask; mask >>= 1) {
        digitalWrite(getData(), LOW);

        if (ENABLE_OPEN_COLLECTOR) {
            pinMode(SDAT, INPUT);
        }

        delayMicroseconds(BIT_GAP_DELAY_MICROSEC);
        // Bit
        if (word & mask) {
            // This is a 1 bit.
//	    digitalWrite(SDAT, LOW);
            delayMicroseconds(BIT_ONE_DELAY_MICROSEC);
        }

        if (ENABLE_OPEN_COLLECTOR) {
            pinMode(SDAT, OUTPUT);
        }

        digitalWrite(getData(), HIGH);
        delayMicroseconds(BIT_TERMINATOR_DELAY_MICROSEC);
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


