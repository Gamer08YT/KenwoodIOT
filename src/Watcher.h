//
// Created by Jan Heil on 05.07.2023.
//

#ifndef KENWOODIOT_WATCHER_H
#define KENWOODIOT_WATCHER_H


// Define Calibration of Current Sensor.
#define calibrationIO 355.55

class Watcher {
public:
    static void prepare();

    static void handleMeasurement();

    static float getIRMS();

    static void setRelais(int pinIO, bool stateIO);

    static void handleRelais();

    static bool getState();

    static void handleRelaisInput(bool stateIO);

    static bool getManuel();
};


#endif //KENWOODIOT_WATCHER_H