//
// Created by Jan Heil on 03.07.2023.
//

#ifndef KENWOODIOT_HELPCOMMAND_H
#define KENWOODIOT_HELPCOMMAND_H


#include <vector>
#include "Command.h"

class HelpCommand : public Command {
public:
    const char *invoke() override {
        return "help";
    }

    const char *description() override {
        return "Display Help Page.";
    }

    void execute(std::vector<String> argsIO);
};


#endif //KENWOODIOT_HELPCOMMAND_H
