//
// Created by Hanh Hoang on 9.2.2025.
//

#ifndef DATASTRUCTUREALGORITHMSPROJECT_DATABASESTORAGE_H
#define DATASTRUCTUREALGORITHMSPROJECT_DATABASESTORAGE_H
#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <thread>
#include <map>
#include <mutex>
#include "QueueManager.h"
#include "InfoNode.h"

using namespace std;
using json = nlohmann::json;

class DatabaseStorage {
public:
    DatabaseStorage(string file_path, QueueManager& queue_manager);
    ~DatabaseStorage();
    json read_database();
    void start_write_thread();
    void stop_write_thread();

private:
    string file_path;
    QueueManager& queue_manager;
    bool running;
    json json_data = json{};
    map<string, map<uint64_t , int>> data_container;
    mutex file_lock;
    thread database_writer;
    void write_database();
};


#endif //DATASTRUCTUREALGORITHMSPROJECT_DATABASESTORAGE_H
