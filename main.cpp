#include <iostream>
#include <nlohmann/json.hpp>
#include "TemperatureSensor.h"
#include "QueueManager.h"
#include "DatabaseStorage.h"
#include "InfoNode.h"

using namespace std;
using json = nlohmann::json;
int main(){
    // string json_file_path = "/Users/HannahHoang/DataStructureAlgorithmProject/database/database.json"; //MUST CHANGE THIS TO LOCAL ABS PATH
    string json_file_path = "/home/hannah/DataStructureAlgorithmProject/database/database.json"; //MUST CHANGE THIS TO LOCAL ABS PATH

    QueueManager queue_manager;
    DatabaseStorage database(json_file_path, queue_manager);
    database.start_write_thread();
    TemperatureSensor temp_sensor1("sensor1", 0,  queue_manager, 3);
    TemperatureSensor temp_sensor2("sensor2",1, queue_manager);
    TemperatureSensor temp_sensor3("sensor3",1, queue_manager, 4);
    temp_sensor1.start_temp_reading_thread();
    temp_sensor2.start_temp_reading_thread();
    temp_sensor3.start_temp_reading_thread();
    this_thread::sleep_for(chrono::seconds(15));

    temp_sensor1.stop_temp_reading_thread();
    temp_sensor2.stop_temp_reading_thread();
    temp_sensor3.stop_temp_reading_thread();
    database.stop_write_thread();
    json j_data;
    j_data = database.read_database();
    cout << "json database  " << j_data.dump(4) << endl;

    //Do search and filter data from database json file
    map<string, map<uint64_t, int>> sensor_data = j_data.get<map<string, map<uint64_t, int>>>();
    for (const auto& [sensor, readings] : sensor_data) {
        cout << "Sensor: " << sensor << endl;
        for (const auto& [timestamp, temp] : readings) {
            if (temp > 3) {
                cout << "  Timestamp: " << timestamp << " -> Temp: " << temp << endl;
            }
        }
    }
    return EXIT_SUCCESS;
}