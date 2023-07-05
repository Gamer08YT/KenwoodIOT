//
// Created by Jan Heil on 05.07.2023.
//

#include <Arduino.h>
#include "Watcher.h"

// Store Max Value.
int maxIO;

// Store New measured Value.
int newIO;

// Store last Value.
int oldIO = 0;

// Store RMS.
int rmsIO;

// Store IRMS.
int irmsIO = 0;

// Store last Update Time.
unsigned long updatedIO = 0;

/**
 * Prepare PINS for Current Sensor.
 */
void Watcher::prepare() {
    // Set PIN A0 as Input.
    pinMode(A0, INPUT);
}

/**
 * Handle Current Measurement.
 */
void Watcher::handleMeasurement() {
    // Give Function 1000ms debounce time.
    if ((millis() - updatedIO) > 1000) {

        // Read new Current Value.
        newIO = analogRead(A0);

        // Check if Current got smaller.
        if (newIO < oldIO) {
            maxIO = oldIO;

            // Reset Int Buffer.
            oldIO = 0;
        }

        // Calculate RMS.
        rmsIO = (maxIO * 5.00 * 0.707 / 1024);

        // Calculate IRMS (mA).
        irmsIO = rmsIO * calibrationIO;

        // Update Timestamp.
        updatedIO = millis();
    }
}

/**
 * Return IRMS Current in mA.
 * @return
 */
int Watcher::getIRMS() {
    return irmsIO;
}

/**
 * Get A0 PIN Value.
 * @return
 */
int Watcher::getAO() {
    return newIO;
}
