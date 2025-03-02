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
//                cout << "Write data " << json_data << " to json write_file " << file_path << endl;
                write_file << json_data.dump(4);
                write_file.close();
            } else {
                cerr << "Can't open write_file " << file_path << " to write" << endl;
            }
        }

        this_thread::sleep_for(chrono::nanoseconds(500) );
    }
    cout << "Stop database writer thread " << endl;
}

void DatabaseStorage::stop_write_thread() {
    running = false;
    if(database_writer.joinable()) database_writer.join();
}


json DatabaseStorage::read_database() {
    lock_guard<mutex> lock(file_lock);
    ifstream read_file(file_path);
    json j;
    if(read_file.is_open()){
        try {
            read_file >> j;
            read_file.close();
        } catch (const std::exception &e) {
            std::cerr << "Error reading JSON: " << e.what() << std::endl;
            return json{}; // Return an empty JSON object on failure
        }
        return j;
    }else{
        cerr << "Can't open " << file_path << " to read data" << endl;
        return json{};
    }
}

pair<map<string, vector<uint64_t>>, map<string, vector<float>>> DatabaseStorage::process_data() {
    auto j = read_database();
    map<string, vector<uint64_t>> sensorTimestamps;
    map<string, vector<float>> sensorValues;

    for (auto& sensor : j.items()) {
        const string& sensorName = sensor.key();  // get the sensor name
        auto& entries = sensor.value();           // get the map of {timestamp: value}
        for (auto& entry : entries.items()) {
            uint64_t timestamp = stoull(entry.key());
            float value = entry.value().get<float>();
            sensorTimestamps[sensorName].push_back(timestamp);
            sensorValues[sensorName].push_back(value);
        }
    }
    return {sensorTimestamps, sensorValues};

}
