//
// Created by Jan Heil on 03.07.2023.
//

#ifndef KENWOODIOT_BYECOMMAND_H
#define KENWOODIOT_BYECOMMAND_H


#include "Command.h"

class ByeCommand : public Command {
public:
    const char *invoke() override {
        return "bye";
    }

    const char *description() override {
        return "Close Telnet Session.";
    }

    void execute(String *argsIO);
};


#endif //KENWOODIOT_BYECOMMAND_H
