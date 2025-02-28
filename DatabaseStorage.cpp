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
            std::lock_guard<std::mutex> lock(file_lock);

            data_container[node.name][node.timestamps] = node.temp;

            // construct the JSON object
            nlohmann::json json_data = nlohmann::json::object();
            for (const auto& [sensorName, readings] : data_container) {
                nlohmann::json sensor_data = nlohmann::json::object();
                for (const auto& [timestamp, temperature] : readings) {
                    sensor_data[std::to_string(timestamp)] = temperature;
                }
                json_data[sensorName] = sensor_data;
            }

            // Write the JSON data to the file
            std::ofstream write_file(file_path);
            if (write_file.is_open()) {
                std::cout << "Writing data to JSON file " << file_path << std::endl;
                write_file << json_data.dump(4); // Pretty-print with 4-space indentation
                write_file.close();
            } else {
                std::cerr << "Unable to open file " << file_path << " for writing" << std::endl;
            }
        }

        std::this_thread::sleep_for(std::chrono::nanoseconds(500));
    }
    std::cout << "Stopping database writer thread" << std::endl;
}


pair<map<string, vector<uint64_t>>, map<string, vector<float>>> DatabaseStorage::read_database() {
    lock_guard<mutex> lock(file_lock);

    map<string, vector<uint64_t>> sensorTimestamps;
    map<string, vector<float>> sensorValues;

    ifstream read_file(file_path);
    json j;
    if (read_file.is_open()) {
        try {
            read_file >> j;  // Parse the JSON file
            for (auto& [sensorName, entries] : j.items()) {
                for (auto& entry : entries) {
                    if (entry.is_array() && entry.size() == 2) {  // Ensure valid format [timestamp, value]
                        uint64_t timestamp = entry[0].get<uint64_t>();
                        float value = entry[1].get<float>();
                        sensorTimestamps[sensorName].push_back(timestamp);
                        sensorValues[sensorName].push_back(value);
                    } else {
                        cerr << "Invalid entry format in JSON for sensor " << sensorName << endl;
                    }
                }
            }
            read_file.close();
        } catch (const std::exception& e) {
            cerr << "Error reading or parsing JSON: " << e.what() << endl;
        }
    } else {
        cerr << "Can't open " << file_path << " to read data" << endl;
    }

    return {sensorTimestamps, sensorValues};
}