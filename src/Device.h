//
// Created by Jan Heil on 02.07.2023.
//

#ifndef KENWOODIOT_DEVICE_H
#define KENWOODIOT_DEVICE_H


class Device {

public:
    static void status_led(int durationIO);

    static void beginTelnet();

    static void println(String dataIO);

    static void print(String dataIO);

    static void handleTelnet();

    static void write(String dataIO);
};


#endif //KENWOODIOT_DEVICE_H
