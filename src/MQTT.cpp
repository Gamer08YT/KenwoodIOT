//
// Created by Jan Heil on 02.07.2023.
//

#include "MQTT.h"
#include "Kenwood.h"

/**
 * On Standby Event.
 * @param state
 * @param sender
 */
void MQTT::onStandby(bool state, HASwitch *sender) {
    Serial.print("Set Standby to ");
    Serial.println(state);

    // Toggle AMP Standby State.
    Kenwood::standby(state);

    sender->setState(state);
}

/**
 * On Volume Change Event.
 * @param state
 * @param sender
 */
void MQTT::onVolume(HANumeric state, HANumber *sender) {
    Serial.print("Set Volume to ");
    Serial.println(state.toInt8());

    sender->setState(state);
}

/**
 * On Mute Event.
 * @param state
 * @param sender
 */
void MQTT::onMute(bool state, HASwitch *sender) {
    Serial.print("Set Mute to ");
    Serial.println(state);

    sender->setState(state);
}

/**
 * On Power Event.
 * @param state
 * @param sender
 */
void MQTT::onPower(bool state, HASwitch *sender) {
    Serial.print("Set Power to ");
    Serial.println(state);

    sender->setState(state);
}

/**
 * On Input Change Event.
 */
void MQTT::onInput(int8_t state, HASelect *sender) {
    Serial.print("Set Input to ");
    Serial.println(state);

    // Set Input Channel.
    Kenwood::setInput(state);

    sender->setState(state);
}

/**
 * On Bus Version Change Event.
 * @param state
 * @param sender
 */
void MQTT::onVersion(int8_t state, HASelect *sender) {
    Serial.print("Set Version to ");
    Serial.println(state);

    // Set Interface of Bus.
    Kenwood::set_interface((state == 0 ? -8 : -16));

    sender->setState(state);
}
