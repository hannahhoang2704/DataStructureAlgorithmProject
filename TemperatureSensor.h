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
#include "InfoNode.h"
#include "QueueManager.h"

using namespace std;
class TemperatureSensor {
public:
    TemperatureSensor(const string name, const string sensorDir, QueueManager& dequeue, int interval=1): name(name), sensorDir(sensorDir), queue_manager(dequeue), interval(interval){
        //open the file only once here when constructing object
        openFile();
    };
    ~TemperatureSensor();

    void start_temp_reading_thread();
    void stop_temp_reading_thread();
    float get_temperature();

private:
    const string name;
    const string sensorDir;
    float temp=0;
    int interval;
    bool terminated = false;
    ifstream sensor_file;
    thread temperature_reader;
    QueueManager & queue_manager;
    void read_temperature();
    void openFile();
};



#endif //DATASTRUCTUREALGORITHMSPROJECT_TEMPERATURESENSOR_H
