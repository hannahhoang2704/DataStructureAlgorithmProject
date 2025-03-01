//
// Created by Hanh Hoang on 7.2.2025.
//

#include "TemperatureSensor.h"
#include "SensorManager.h"

#define SENSORS_PATH "/sys/bus/w1/devices/"

void TemperatureSensor::openFile() {
    sensor_file.open(SENSORS_PATH + file_name + "/w1_slave");
    if (!sensor_file.is_open()) {
        cerr << "Error: Could not open " << SENSORS_PATH + file_name << endl;
    }
}

TemperatureSensor::~TemperatureSensor() {
    stop_temp_reading_thread();
}

void TemperatureSensor::start_temp_reading_thread() {
    cout << "Reading temperature from "<< name << " every " << interval << " seconds"<< endl;
    if (temperature_reader.joinable()) {
        terminated = true;
        temperature_reader.join();
    }

    terminated = false;
    temperature_reader= thread(&TemperatureSensor::read_temperature, this);
}

void TemperatureSensor::stop_temp_reading_thread() {
    cout << "Stop temperature reading thread for " << name << endl;
    terminated = true;
    if(temperature_reader.joinable()) temperature_reader.join();
    if (sensor_file.is_open()) {
        sensor_file.close();  //close file once thread terminates
    }
}

void TemperatureSensor::read_temperature() {
    while(!terminated){
        uint64_t timestamp = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count();
        auto t_c = static_cast<time_t>(timestamp);

        lock_guard<mutex> temp_lock(temp_mutex);
        temp = get_temperature();

        if (temp == -1) {
            cerr << "Error: Failed to read temperature from: " << name << endl;
        } else {
            if (!is_initialized) {
                is_initialized = true;
            }
            InfoNode node(name, timestamp, temp);
            cout << "[" << put_time(localtime(&t_c), "%Y-%m-%d %H:%M:%S") << "]\t" << name << ": " << temp << " °C" << endl;
            queue_manager.push_back(node);
        }
        this_thread::sleep_for(chrono::seconds(interval));
    }
}

float TemperatureSensor::get_temperature() {
    lock_guard<mutex> lock(file_mutex);
    if (!sensor_file.is_open()) {
        cerr << "Reopening sensor file for " << name << endl;
        openFile();
        if (!sensor_file.is_open()) {
            cerr << "ERROR: Could not reopen sensor file for " << name << endl;
            return -1;
        }
    }

    string line, temp_data;
    while (getline(sensor_file, line)) {
        if (line.find("t=") != string::npos) {
            temp_data = line.substr(line.find("t=") + 2);
        }
    }
    //reset file position for next read
    sensor_file.clear();
    sensor_file.seekg(0, ios::beg);

    if (!temp_data.empty()) {
        return stoi(temp_data) / 1000.0;
    }

    return -1;
}

