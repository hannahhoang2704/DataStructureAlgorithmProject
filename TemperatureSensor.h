//
// Created by Hanh Hoang on 7.2.2025.
//

#ifndef DATASTRUCTUREALGORITHMSPROJECT_TEMPERATURESENSOR_H
#define DATASTRUCTUREALGORITHMSPROJECT_TEMPERATURESENSOR_H

#define SENSORS_PATH "/sys/bus/w1/devices/"

#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <atomic>
#include <utility>
#include "InfoNode.h"
#include "QueueManager.h"
#include "SensorManager.h"
#include "InfoNode.h"

using namespace std;
class TemperatureSensor {
public:
    TemperatureSensor(string  name, string  sensor_file_name, QueueManager& dequeue, int interval=1): name(std::move(name)), file_name(std::move(sensor_file_name)), queue_manager(dequeue), interval(interval), is_initialized(false){
        openFile();
    };
    ~TemperatureSensor();

    void start_temp_reading_thread();
    void stop_temp_reading_thread();
    float get_temperature();
    atomic<bool> is_initialized; //semaphore to notify UI that the sensor is initialized

private:
    const string name;
    const string file_name;
    float temp=0.0;
    int interval;
    bool terminated = false;
    mutex file_mutex;
    mutex temp_mutex;
    ifstream sensor_file;
    thread temperature_reader;
    QueueManager & queue_manager;
    void read_temperature();
    void openFile();
};



#endif //DATASTRUCTUREALGORITHMSPROJECT_TEMPERATURESENSOR_H
