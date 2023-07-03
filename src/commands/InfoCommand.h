//
// Created by Jan Heil on 03.07.2023.
//

#ifndef KENWOODIOT_INFOCOMMAND_H
#define KENWOODIOT_INFOCOMMAND_H


#include <vector>
#include "Command.h"

class InfoCommand : public Command {
public:
    const char* invoke() override {
        return "info";
    }

    const char* description() override {
        return "Get Device Information.";
    }

    void execute(std::vector<String> argsIO);
};


#endif //KENWOODIOT_INFOCOMMAND_H
