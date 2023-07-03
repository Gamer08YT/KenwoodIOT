//
// Created by Jan Heil on 03.07.2023.
//

#include "ByeCommand.h"
#include "Device.h"

void ByeCommand::execute(String *argsIO) {
    // Print Goodbye Message.
    Device::println("Goodbye, have a nice Day ;-)");

    // Close Telnet Socket.
    Device::getTelnet().disconnectClient(true);
}
// 