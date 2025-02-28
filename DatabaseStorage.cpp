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
            data_container[node.name].push_back({node.timestamps, node.temp});

            ofstream write_file(file_path);
            if(write_file.is_open()){
                json_data = json(data_container);
                cout << "Write data " << json_data << " to json write_file " << file_path << endl;
                write_file << json_data.dump(4);
                write_file.close();
            }else{
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

/*json DatabaseStorage::read_database() {
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
}*/

pair<map<string, vector<uint64_t>>, map<string, vector<float>>> DatabaseStorage::read_database() {
    lock_guard<mutex> lock(file_lock);
    map<string, vector<uint64_t>> sensorTimestamps;
    map<string, vector<float>> sensorValues;

    ifstream read_file(file_path);
    json j;
    if (read_file.is_open()) {
        try {
            read_file >> j;
            for (auto& sensor : j.items()) {
                const string& sensorName = sensor.key();  // get the sensor name
                auto& entries = sensor.value();          // get the array of [timestamp, value]

                // Iterate through the array of [timestamp, value]
                for (auto& entry : entries) {
                    if (entry.is_array() && entry.size() == 2) {
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
            std::cerr << "Error reading or parsing JSON: " << e.what() << std::endl;
        }
    } else {
        cerr << "Can't open " << file_path << " to read data" << endl;
    }

    return {sensorTimestamps, sensorValues};
}