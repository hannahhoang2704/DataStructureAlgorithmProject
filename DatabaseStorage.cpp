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

void DatabaseStorage::stop_write_thread() {
    running = false;
    if(database_writer.joinable()) database_writer.join();
}

void DatabaseStorage::write_database() {
    while (running) {
        InfoNode node;
        if (queue_manager.pop_data(node)) {
            lock_guard<mutex> lock(file_lock);
            data_container[node.name][node.timestamps] = node.temp;

            // Open file for writing
            ofstream write_file(file_path);
            if (write_file.is_open()) {
                // Start constructing the JSON manually
                nlohmann::json json_data;
                for (const auto& [sensorName, readings] : data_container) {
                    nlohmann::json sensor_data; // JSON object for each sensor
                    for (const auto& [timestamp, temperature] : readings) {
                        // Manually convert uint64_t timestamp to string to avoid issues
                        sensor_data[std::to_string(timestamp)] = temperature;
                    }
                    // Assign the sensor data object to the main JSON
                    json_data[sensorName] = sensor_data;
                }
                std::cout << "Write data " << json_data.dump(4) << " to json write_file " << file_path << std::endl;
                write_file << json_data.dump(4);  // Pretty-print with 4 spaces
                write_file.close();
            } else {
                cerr << "Can't open write_file " << file_path << " to write" << endl;
            }
        }

        // Sleep briefly before attempting to process the next node
        this_thread::sleep_for(chrono::nanoseconds(500));
    }
    cout << "Stop database writer thread " << endl;
}


pair<map<string, vector<uint64_t>>, map<string, vector<float>>> DatabaseStorage::read_database() {
    lock_guard<mutex> lock(file_lock);
    map<string, vector<uint64_t>> sensorTimestamps;
    map<string, vector<float>> sensorValues;

    ifstream read_file(file_path);
    json j;
    if (read_file.is_open()) {
        try {
            read_file >> j;  // Parse JSON from file
            for (auto& [sensorName, readings] : j.items()) {
                for (auto& [timestampStr, temperature] : readings.items()) {
                    // Manually convert JSON string keys back to uint64_t and float values
                    uint64_t timestamp = std::stoull(timestampStr);
                    float value = temperature.get<float>();
                    sensorTimestamps[sensorName].push_back(timestamp);
                    sensorValues[sensorName].push_back(value);
                }
            }
            read_file.close();
        } catch (const std::exception& e) {
            std::cerr << "Error reading or parsing JSON: " << e.what() << std::endl;
        }
    } else {
        cerr << "Can't open " << file_path << " to read data" << endl;
    }

    return {sensorTimestamps, sensorValues};
}