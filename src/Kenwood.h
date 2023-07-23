//
// Created by Jan Heil on 02.07.2023.
//

#ifndef KENWOODIOT_KENWOOD_H
#define KENWOODIOT_KENWOOD_H


#include <cstdint>

class Kenwood {
public:

    static void prepare();

    static void standby(bool state);

    static void set_interface(int i);

    static void try_all(int wait);

    static void handleCommand(int valueIO);

    static int getInterface();

    static void setInput(int state);

    static uint8_t getData();

    static uint8_t getBusy();

    static void sendWord(unsigned long word);

    static void send_cmd(unsigned long cmd);
};


#endif //KENWOODIOT_KENWOOD_H
