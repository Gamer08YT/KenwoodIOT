//
// Created by Jan Heil on 03.07.2023.
//

#include "SendCommand.h"
#include "Device.h"

void SendCommand::execute(std::vector<String> argsIO) {
    // Check for right Size.
    if (argsIO.size() != 0) {
        if (argsIO.size() == 1) {
            // Print Wait Message.
            Device::println("Waiting for sending BUS-Command.");
        } else {
            // Print to many Argument Message.
            Device::println("Too many Arguments given, you can only send one Command at once.");
        }
    } else {
        // Print Missing Argument Message.
        Device::println("Please use /send <BUS-Command>.");
    }
}
