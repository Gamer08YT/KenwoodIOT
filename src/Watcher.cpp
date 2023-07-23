//
// Created by Jan Heil on 05.07.2023.
//

#include <Arduino.h>
#include "Watcher.h"
#include "EmonLib.h"
#include "Device.h"
#include "Kenwood.h"

// Store Energy Monitor Instance.
EnergyMonitor monitor;

// Store IRMS.
float irmsIO = 0;

// Store last Update Time.
unsigned long updatedIO = 0;

// Store Lock for Relais.
int counterIO = 0;

// Store Relais State.
bool stateIO = false;

// Store Manuel Mode.
bool manuelIO = false;

/**
 * Prepare PINS for Current Sensor.
 */
void Watcher::prepare() {
    // Set PIN A0 as Input.
    pinMode(A0, INPUT);

    // Set PINs for Relais.
    pinMode(05, OUTPUT);
    pinMode(04, OUTPUT);

    // Set PINS for Relais LOW.
    setRelais(05, false);
    setRelais(04, false);

    // Setup Monitor.
    monitor.current(A0, 444.4);
}

/**
 * Handle Current Measurement.
 */
void Watcher::handleMeasurement() {

    // Give Function 1000ms debounce time.
    if ((millis() - updatedIO) > 1000) {
        // Calc only IRMS in A.
        irmsIO = (monitor.calcIrms(1480) / 1000);

        // Handle Relais.
        Watcher::handleRelais();

        // Update Timestamp.
        updatedIO = millis();
    }
}

/**
 * Return IRMS Current in mA.
 * @return
 */
float Watcher::getIRMS() {
    return irmsIO;
}

/**
 * Set Pin State of given Relais.
 * Need to invert because when Pulled to Ground they activate.
 * @param pinIO
 * @param stateIO
 */
void Watcher::setRelais(int pinIO, bool stateIO) {
    Device::print("Switch Relais State ");
    Device::print(String(pinIO));
    Device::print(" to ");
    Device::println(String(!stateIO));

    // Write to Pin.
    digitalWrite(pinIO, (!stateIO ? HIGH : LOW));
}

/**
 * Handle Relais by defined Trigger Point.
 */
void Watcher::handleRelais() {
    // Handle Trigger with measured Load.
    if (static_cast<float>(Device::getTrigger()) < (irmsIO * 230)) {
        if (!stateIO && !manuelIO) {
            // Wait 1 Second to prevent Power/Current Spike.
            Device::status_led(1000);

            // Enable Relais for Socket.
            Watcher::setRelais(05, true);

            // Reset Counter.
            counterIO = 0;

            // Set Relais State.
            stateIO = true;
        }
    } else {
        if (stateIO && !manuelIO) {
            // Check if Counter is smaller than 5 Seconds to give a small debounce time..
            if (counterIO < 5) {
                // Set LED Status.
                Device::status_led(250);

                // Increment Counter.
                counterIO++;
            } else {
                // Set LED Status.
                Device::status_led(500);

                // Disable Relais for Socket after a while.
                Watcher::setRelais(05, false);

                // Reset Counter.
                counterIO = 0;

                // Set Relais State.
                stateIO = false;
            }
        }
    }
}

bool Watcher::getState() {
    return stateIO;
}

/**
 * Handle Relais Manual Mode.
 * @param stateIO
 */
void Watcher::handleRelaisInput(bool valueIO) {
    if (!stateIO && valueIO) {
        // Enable Manuel Mode.
        manuelIO = true;

        // Set Power State of Relais.
        Watcher::setRelais(05, true);

        // Print Debug Message.
        Device::println("Entered Manuel Mode.");

        // Set Relais State.
        stateIO = true;
    } else if (stateIO && !valueIO) {
        // Disable Manual Mode.
        manuelIO = false;

        // Print Debug Message.
        Device::println("Quitting Manuel Mode.");

        // Relais getting Switched via Watcher Automatism.
    }
}

bool Watcher::getManuel() {
    return manuelIO;
}
