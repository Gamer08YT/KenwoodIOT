//
// Created by Jan Heil on 03.07.2023.
//

#include "ResetCommand.h"
#include "Device.h"

void ResetCommand::execute(std::vector<String> argsIO) {
    // Print Debug Message.
    Device::println("Device will reset...");

    // Wait 500ms.
    delay(500);

    // Reset ESP.
    ESP.reset();
}