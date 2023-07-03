//
// Created by Jan Heil on 03.07.2023.
//

#ifndef KENWOODIOT_INFOCOMMAND_H
#define KENWOODIOT_INFOCOMMAND_H


#include "Command.h"

class InfoCommand : public Command {
    String invoke = "info";

    String description = "Displays Debug Information.";
};


#endif //KENWOODIOT_INFOCOMMAND_H
