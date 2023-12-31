//
// Created by Jan Heil on 02.07.2023.
//


#ifndef KENWOODIOT_DEVICE_H
#define KENWOODIOT_DEVICE_H

#include "ESPTelnetBase.h"
#include "ESPTelnet.h"
#include "commands/Command.h"
#include "EscapeCodes.h"
#include "HAMqtt.h"
#include "IotWebConf.h"


class Device {

public:
    static void status_led(int durationIO);

    static void beginTelnet();

    static void println(String dataIO);

    static void print(String dataIO);

    static void handleTelnet();

    static void write(String dataIO);

    static void onConnect(String addressIO);

    static void onDisconnect(String addressIO);

    static void onInput(String dataIO);

    static ESPTelnet getTelnet();

    static EscapeCodes getANSI();

    static std::vector<Command *> getCommands();

    static void addCommand(Command *commandIO);

    static void addCommands();

    static void handleCommand(String dataIO);

    static void print_device();

    static std::vector<String> split(String &valueIO, const char *delimiterIO = "");

    static void beginOTA();

    static void beginWebserver();

    static void handleWebserver();

    static WiFiClient getWiFi();

    static WebServer getServer();

    static void beginConfig();

    static void handleConfig();

    static int getTrigger();

    static void setTrigger(uint16_t valueIO);

    static void setType(int8_t state);

    static void setInput(int8_t state);

    static const int8_t getInput();

    static const int8_t getType();

    static void onWiFiConnected();
};


#endif //KENWOODIOT_DEVICE_H
