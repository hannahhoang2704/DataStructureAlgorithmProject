//
// Created by Hanh Hoang on 7.2.2025.
//

#include "TemperatureSensor.h"

TemperatureSensor::~TemperatureSensor() {
    stop_temp_reading_thread();
}

void TemperatureSensor::start_temp_reading_thread() {
    cout << "Reading temperature in every " << interval << "seconds"<< endl;
    temperature_reader= thread(&TemperatureSensor::read_temperature, this);
}

void TemperatureSensor::stop_temp_reading_thread() {
    cout << "Stop temperature reading thread" << endl;
    terminated = true;
    temperature_reader.join();
}

void TemperatureSensor::read_temperature() {
    while(!terminated){
        cout << "Reading..." << endl;
        //Reading temperature part needs to add here
        cout << "Temperature is " << temp << endl;
        this_thread::sleep_for(chrono::seconds(interval));
    }
}

int TemperatureSensor::get_temperature() {
    return temp;
}
