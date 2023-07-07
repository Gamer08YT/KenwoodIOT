//
// Created by Jan Heil on 05.07.2023.
//

#include <Arduino.h>
#include "Watcher.h"
#include "EmonLib.h"

// Store Energy Monitor Instance.
EnergyMonitor monitor;

// Store IRMS.
float irmsIO = 0;

// Store last Update Time.
unsigned long updatedIO = 0;

/**
 * Prepare PINS for Current Sensor.
 */
void Watcher::prepare() {
    // Set PIN A0 as Input.
    pinMode(A0, INPUT);

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
