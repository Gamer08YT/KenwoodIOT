//
// Created by Jan Heil on 02.07.2023.
//

#include "MQTT.h"
#include "Kenwood.h"
#include "Device.h"

/**
 * On Standby Event.
 * @param state
 * @param sender
 */
void MQTT::onStandby(bool state, HASwitch *sender) {
    Device::print("Set Standby to ");
    Device::println(String(state));

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
    Device::print("Set Volume to ");
    Device::println(String(state.toInt8()));

    sender->setState(state);
}

/**
 * On Mute Event.
 * @param state
 * @param sender
 */
void MQTT::onMute(bool state, HASwitch *sender) {
    Device::print("Set Mute to ");
    Device::println(String(state));

    sender->setState(state);
}

/**
 * On Power Event.
 * @param state
 * @param sender
 */
void MQTT::onPower(bool state, HASwitch *sender) {
    Device::print("Set Power to ");
    Device::println(String(state));

    sender->setState(state);
}

/**
 * On Input Change Event.
 */
void MQTT::onInput(int8_t state, HASelect *sender) {
    Device::print("Set Input to ");
    Device::println(String(state));

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
    Device::print("Set Version to ");
    Device::println(String(state));

    // Set Interface of Bus.
    Kenwood::set_interface((state == 0 ? -8 : -16));

    sender->setState(state);
}

/**
 * On Reset Button Press.
 * @param button
 */
void MQTT::onReset(HAButton *button) {
    // Print Debug Information.
    Device::println("Resetting TV Power.");
}
