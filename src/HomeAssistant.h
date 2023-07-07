//
// Created by Jan Heil on 02.07.2023.
//

#ifndef KENWOODIOT_HOMEASSISTANT_H
#define KENWOODIOT_HOMEASSISTANT_H

#include <WiFiClient.h>
#include "HADevice.h"
#include "device-types/HANumber.h"

class HomeAssistant {

public:
    static WiFiClient client();

    static void prepare();
};


#endif //KENWOODIOT_HOMEASSISTANT_H
