//
// Created by Jan Heil on 03.07.2023.
//

#ifndef KENWOODIOT_SENDCOMMAND_H
#define KENWOODIOT_SENDCOMMAND_H


#include <vector>
#include "Command.h"

class SendCommand : public Command {
public:
    const char *invoke() override {
        return "send";
    }

    const char *description() override {
        return "Send Data to Kenwood Bus.";
    }

    void execute(std::vector<String> argsIO);
};

#endif //KENWOODIOT_SENDCOMMAND_H
