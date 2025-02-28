//
// Created by Hanh Hoang on 9.2.2025.
//

#include "DatabaseStorage.h"

DatabaseStorage::DatabaseStorage(string file_path, QueueManager &queue_manager): file_path(file_path), queue_manager(queue_manager) {
    running = false;
}

DatabaseStorage::~DatabaseStorage() {
    stop_write_thread();
}

void DatabaseStorage::start_write_thread() {
    running = true;
    database_writer = thread(&DatabaseStorage::write_database, this);
    cout << "Start database writer thread " << endl;
}

void DatabaseStorage::write_database() {
    while(running){
        InfoNode node;
        if(queue_manager.pop_data(node)){
            lock_guard<mutex> lock(file_lock);
            data_container[node.name][node.timestamps] = node.temp;

            ofstream write_file(file_path);
            if (write_file.is_open()) {
                json json_data;
                for (const auto& sensor : data_container) {
                    json sensor_data;
                    for (const auto& entry : sensor.second) {
                        sensor_data[std::to_string(entry.first)] = entry.second;
                    }
                    json_data[sensor.first] = sensor_data;
                }
                cout << "Write data " << json_data << " to json write_file " << file_path << endl;
                write_file << json_data.dump(4);
                write_file.close();
            } else {
                cerr << "Can't open write_file " << file_path << " to write" << endl;
            }
        }
//            if(write_file.is_open()){
//                json_data = json(data_container);
//                cout << "Write data " << json_data << " to json write_file " << file_path << endl;
//                write_file << json_data.dump(4);
//                write_file.close();
//            }else{
//                cerr << "Can't open write_file " << file_path << " to write" << endl;
//            }
//        }

        this_thread::sleep_for(chrono::nanoseconds(500) );
    }
    cout << "Stop database writer thread " << endl;
}

void DatabaseStorage::stop_write_thread() {
    running = false;
    if(database_writer.joinable()) database_writer.join();
}


//pair<map<string, vector<uint64_t>>, map<string, vector<float>>> DatabaseStorage::read_database() {
//    lock_guard<mutex> lock(file_lock);
//    map<string, vector<uint64_t>> sensorTimestamps;
//    map<string, vector<float>> sensorValues;
//
//    ifstream read_file(file_path);
//    json j;
//    if (read_file.is_open()) {
//        try {
//            read_file >> j;
//            for (auto& sensor : j.items()) {
//                const string& sensorName = sensor.key();  // get the sensor name
//                auto& entries = sensor.value();          // get the array of [timestamp, value]
//
//                // Iterate through the array of [timestamp, value]
//                for (auto& entry : entries) {
//                    if (entry.is_array() && entry.size() == 2) {
//                        uint64_t timestamp = entry[0].get<uint64_t>();
//                        float value = entry[1].get<float>();
//                        sensorTimestamps[sensorName].push_back(timestamp);
//                        sensorValues[sensorName].push_back(value);
//                    } else {
//                        cerr << "Invalid entry format in JSON for sensor " << sensorName << endl;
//                    }
//                }
//            }
//            read_file.close();
//        } catch (const std::exception& e) {
//            std::cerr << "Error reading or parsing JSON: " << e.what() << std::endl;
//        }
//    } else {
//        cerr << "Can't open " << file_path << " to read data" << endl;
//    }
//
//    return {sensorTimestamps, sensorValues};
//}

pair<map<string, vector<uint64_t>>, map<string, vector<float>>> DatabaseStorage::read_database() {
    lock_guard<mutex> lock(file_lock);
    map<string, vector<uint64_t>> sensorTimestamps;
    map<string, vector<float>> sensorValues;

    ifstream read_file(file_path);
    if (!read_file.is_open()) {
        cerr << "Can't open " << file_path << " to read data" << endl;
        return {sensorTimestamps, sensorValues};
    }

    json j;
    try {
        read_file >> j;
        for (auto& sensor : j.items()) {
            const string& sensorName = sensor.key();  // get the sensor name
            auto& entries = sensor.value();          // get the map of {timestamp: value}

            for (auto& entry : entries.items()) {
                uint64_t timestamp = stoull(entry.key());
                float value = entry.value().get<float>();
                sensorTimestamps[sensorName][timestamp] = value;
                sensorValues[sensorName][timestamp] = value;
            }
        }
    } catch (const std::exception& e) {
        cerr << "Error reading or parsing JSON: " << e.what() << endl;
    }

    read_file.close();
    return {sensorTimestamps, sensorValues};
}


void DatabaseStorage::preparePlotData(
        const std::string &sensorName,
        const map<std::string, vector<uint64_t>> &timestamps,
        const map<std::string, vector<float>> &values,
        vector<float> &timeInSeconds,
        vector<float> &sensorValues
) {
    timeInSeconds.clear();
    sensorValues.clear();

    auto timeIter = timestamps.find(sensorName);
    auto valueIter = values.find(sensorName);

    if (timeIter == timestamps.end() || valueIter == values.end()) {
        std::cerr << "Error: Sensor data not found for " << sensorName << std::endl;
        return;
    }

    const vector<uint64_t>& rawTimestamps = timeIter->second;
    const vector<float>& rawValues = valueIter->second;

    if (rawTimestamps.empty() || rawValues.empty()) {
        std::cerr << "Error: Sensor data is empty for " << sensorName << std::endl;
        return;
    }

    // Normalize timestamps to start from 0
    uint64_t startTime = rawTimestamps.front();
    for (size_t i = 0; i < rawTimestamps.size() && i < rawValues.size(); ++i) {
        timeInSeconds.push_back((rawTimestamps[i] - startTime) / 1000.0f); // Convert to seconds
        sensorValues.push_back(rawValues[i]);
    }
}
