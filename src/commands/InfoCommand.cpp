//
// Created by Jan Heil on 03.07.2023.
//

#include "InfoCommand.h"
#include "Device.h"

void InfoCommand::execute(std::vector<String> argsIO) {
    Device::print("Args given: ");
    Device::println(String(argsIO.size()));

    // Print Device Info.
    Device::print_device();
}
