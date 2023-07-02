//
// Created by Jan Heil on 02.07.2023.
//

#include <Arduino.h>
#include "Device.h"

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
