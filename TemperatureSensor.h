//
// Created by Hanh Hoang on 7.2.2025.
//

#ifndef DATASTRUCTUREALGORITHMSPROJECT_TEMPERATURESENSOR_H
#define DATASTRUCTUREALGORITHMSPROJECT_TEMPERATURESENSOR_H


#include <iostream>
#include <thread>
#include <chrono>

using namespace std;
class TemperatureSensor {
public:
    TemperatureSensor(uint32_t gpio, int interval=1): gpio(gpio), interval(interval){};
    ~TemperatureSensor();
    void start_temp_reading_thread();
    void stop_temp_reading_thread();
    int get_temperature();
private:
    uint32_t gpio;
    float temp=0;
    int interval;
    void read_temperature();
    bool terminated = false;
    thread temperature_reader;
};



#endif //DATASTRUCTUREALGORITHMSPROJECT_TEMPERATURESENSOR_H
