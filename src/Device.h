//
// Created by Jan Heil on 02.07.2023.
//


#ifndef KENWOODIOT_DEVICE_H
#define KENWOODIOT_DEVICE_H

#include "ESPTelnetBase.h"
#include "ESPTelnet.h"
#include "commands/Command.h"
#include "EscapeCodes.h"


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
};


#endif //KENWOODIOT_DEVICE_H
