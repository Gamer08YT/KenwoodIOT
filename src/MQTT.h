//
// Created by Jan Heil on 02.07.2023.
//

#ifndef KENWOODIOT_MQTT_H
#define KENWOODIOT_MQTT_H

#include <device-types/HASwitch.h>
#include "utils/HANumeric.h"
#include "device-types/HANumber.h"
#include "device-types/HASelect.h"
#include "device-types/HAButton.h"

class MQTT {
public:
    static void onStandby(bool state, HASwitch *sender);

    static void onPower(bool state, HASwitch *sender);

    static void onMute(bool state, HASwitch *sender);

    static void onVolume(HANumeric state, HANumber *sender);

    static void onInput(int8_t state, HASelect *sender);

    static void onVersion(int8_t state, HASelect *sender);

    static void onReset(HAButton *button);

    static void onTrigger(HANumeric state, HANumber *sender);
};


#endif //KENWOODIOT_MQTT_H
