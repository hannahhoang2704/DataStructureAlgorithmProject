//
// Created by andig on 19/02/2025.
//

#include "SensorManager.h"

void SensorManager::addSensor(TemperatureSensor* sensor) {
    sensors.push_back(sensor);
}

void SensorManager::startAll() {
    for (auto sensor : sensors) {
        sensor->start_temp_reading_thread();
    }
    unique_lock<mutex> lock(init_mutex);
    if (!convar_initialized.wait_for(lock, chrono::seconds(10), [this]() {
        return allInitialized();
    })) {
        cerr << "Error: Not all sensors were initialized within the timeout period." << endl;
    }
}

void SensorManager::stopAll() {
    for (auto sensor : sensors) {
        sensor->stop_temp_reading_thread();
    }
    cout << "All sensor threads stopped." << endl;
}

bool SensorManager::allInitialized() {
    for (auto sensor : sensors) {
        if (!sensor->is_initialized) {
            return false;
        }
    }
    return true;
}