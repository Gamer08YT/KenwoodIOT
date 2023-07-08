//
// Created by Jan Heil on 02.07.2023.
//

#include "MQTT.h"
#include "Kenwood.h"
#include "Device.h"
#include "Watcher.h"

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

    // Set Power State of Relais.
    Watcher::setRelais(D5, state);

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

    // Change Input Value.
    Device::setInput(state);

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

    // Change Version Value.
    Device::setType(state);

    sender->setState(state);
}

/**
 * On Reset Button Press.
 * @param button
 */
void MQTT::onReset(HAButton *button) {
    // Print Debug Information.
    Device::println("Resetting TV Power.");

    // Retain Button.
    button->setRetain(true);

    // Power off TV.
    Watcher::setRelais(D6, true);

    // Wait 1 Second before Power on.
    delay(2000);

    // Power on TV.
    Watcher::setRelais(D6, false);

    // Retain Button.
    button->setRetain(false);
}

/**
 * On Trigger Change.
 * @param state
 * @param sender
 */
void MQTT::onTrigger(HANumeric state, HANumber *sender) {
    // Change Trigger Value.
    Device::setTrigger(state.toUInt16());

    // Set State of Trigger.
    sender->setState(state);
}
