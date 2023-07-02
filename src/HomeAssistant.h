//
// Created by Jan Heil on 02.07.2023.
//

#ifndef KENWOODIOT_HOMEASSISTANT_H
#define KENWOODIOT_HOMEASSISTANT_H


#include <cstdint>

class HomeAssistant {

};

void connectWiFi();

void connectMQTT();

void prepareComponents();

void onConnected();

void onMessage(const char *topic, const uint8_t *payload, uint16_t length);

void prepare();


#endif //KENWOODIOT_HOMEASSISTANT_H
