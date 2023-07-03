//
// Created by Jan Heil on 03.07.2023.
//

#ifndef KENWOODIOT_COMMAND_H
#define KENWOODIOT_COMMAND_H


#include <WString.h>

class Command {
public:
    virtual const char *invoke() {
        return "empty";
    }

    virtual const char *description() {
        return "Empty Description.";
    }

    void execute(String argsIO[]);
};


#endif //KENWOODIOT_COMMAND_H
