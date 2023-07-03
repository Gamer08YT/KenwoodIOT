//
// Created by Jan Heil on 03.07.2023.
//

#include "ClearCommand.h"
#include "Device.h"


void ClearCommand::execute(std::vector<String> argsIO) {
    // Send Clear Console.
    Device::getTelnet().print(Device::getANSI().cls());
    Device::getTelnet().print(Device::getANSI().home());
}
