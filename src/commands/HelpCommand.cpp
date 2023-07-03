//
// Created by Jan Heil on 03.07.2023.
//

#include "HelpCommand.h"
#include "Device.h"

void HelpCommand::execute(String *argsIO) {
    // Loop trough Commands and Check or Execute.
    for (Command *commandIO: Device::getCommands()) {
        // Print Invoke with Description.
        Device::print(commandIO->invoke());
        Device::print(" -> ");
        Device::println(commandIO->description());
    }
}
