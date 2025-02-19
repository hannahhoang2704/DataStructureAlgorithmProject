//
// Created by andig on 19/02/2025.
//

#ifndef DATASTRUCTUREALGORITHMSPROJECT_SENSORMANAGER_H
#define DATASTRUCTUREALGORITHMSPROJECT_SENSORMANAGER_H
#include "TemperatureSensor.h"

using namespace std;

class SensorManager {
    vector<TemperatureSensor*> sensors;
    mutex init_mutex;
    condition_variable convar_initialized;

public:
    void addSensor(TemperatureSensor* sensor);
    void startAll();
    void stopAll();
    bool allInitialized();
};

#endif //DATASTRUCTUREALGORITHMSPROJECT_SENSORMANAGER_H
