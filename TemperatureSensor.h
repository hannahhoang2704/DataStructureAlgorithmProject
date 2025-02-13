//
// Created by Hanh Hoang on 7.2.2025.
//

#ifndef DATASTRUCTUREALGORITHMSPROJECT_TEMPERATURESENSOR_H
#define DATASTRUCTUREALGORITHMSPROJECT_TEMPERATURESENSOR_H


#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <iomanip>
#include "InfoNode.h"
#include "QueueManager.h"

using namespace std;
class TemperatureSensor {
public:
    TemperatureSensor(const string name, const string sensorDir, QueueManager& dequeue, int interval=1): name(name), sensorDir(sensorDir), queue_manager(dequeue), interval(interval){
        //TODO: the best is to open the file only once here when constructing class
        //readFile();
    };
    ~TemperatureSensor();
    void start_temp_reading_thread();
    void stop_temp_reading_thread();
    int get_temperature();
private:
    const string name;
    const string sensorDir;
    float temp=0;
    int interval;
    bool terminated = false;
    thread temperature_reader;
    QueueManager & queue_manager;
    void read_temperature();
    int read_sensor();
    void readFile();
};



#endif //DATASTRUCTUREALGORITHMSPROJECT_TEMPERATURESENSOR_H
