//
// Created by Hanh Hoang on 7.2.2025.
//

#include "TemperatureSensor.h"
#include "InfoNode.h"

TemperatureSensor::~TemperatureSensor() {
    stop_temp_reading_thread();
}

void TemperatureSensor::start_temp_reading_thread() {
    cout << "Reading temperature from "<< name << " every " << interval << " seconds"<< endl;
    terminated = false;
    temperature_reader= thread(&TemperatureSensor::read_temperature, this);
}

void TemperatureSensor::stop_temp_reading_thread() {
    cout << "Stop temperature reading thread " << temperature_reader.get_id() <<endl;
    terminated = true;
    if(temperature_reader.joinable()) temperature_reader.join();
    //TODO:close the file here once thread terminated
}

void TemperatureSensor::read_temperature() {
    while(!terminated){
        uint64_t timestamp = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count();
        auto t_c = static_cast<time_t>(timestamp);
        read_sensor();
        InfoNode node(name, timestamp, temp);
        cout << "[" << put_time(localtime(&t_c), "%Y-%m-%d %H:%M:%S") << "]\t" << name << ": " << temp <<  " degree"<< endl;
        queue_manager.push_back(node);
        this_thread::sleep_for(chrono::seconds(interval));
    }
    cout << "Thread " << temperature_reader.get_id() << " terminated" << endl;
}

int TemperatureSensor::get_temperature() {
    return temp;
}

int TemperatureSensor::read_sensor(){
    //todo read the last number and do calc
    return temp++; //replace this temp simulator
}