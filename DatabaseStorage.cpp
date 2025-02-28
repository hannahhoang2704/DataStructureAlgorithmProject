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
    while (running) {
        InfoNode node;
        if (queue_manager.pop_data(node)) {
            lock_guard<mutex> lock(file_lock);

            // Add temperature-timestamp pair to the vector
            data_container[node.name].push_back({node.timestamps, node.temp});

            // Serialize to JSON
            json_data = json::object();
            for (const auto& [sensorName, entries] : data_container) {
                json_data[sensorName] = json::array();
                for (const auto& [timestamp, temp] : entries) {
                    json_data[sensorName].push_back({timestamp, temp});
                }
            }

            // Write data to the JSON file
            ofstream write_file(file_path);
            if (write_file.is_open()) {
                cout << "Write data " << json_data << " to JSON file " << file_path << endl;
                write_file << json_data.dump(4);
                write_file.close();
            } else {
                cerr << "Can't open write_file " << file_path << " to write" << endl;
            }
        }

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