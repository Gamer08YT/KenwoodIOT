//
// Created by Jan Heil on 03.07.2023.
//

#include "InfoCommand.h"
#include "Device.h"

void InfoCommand::execute(String *argsIO) {
    Device::println("Info found");
}
