//
// Created by Jan Heil on 03.07.2023.
//

#ifndef KENWOODIOT_CLEARCOMMAND_H
#define KENWOODIOT_CLEARCOMMAND_H


#include <vector>
#include "Command.h"

class ClearCommand : public Command {
public:
    const char *invoke() override {
        return "clear";
    }

    const char *description() override {
        return "Clear Telnet Console.";
    }

    void execute(std::vector<String> argsIO);
};

#endif //KENWOODIOT_CLEARCOMMAND_H
