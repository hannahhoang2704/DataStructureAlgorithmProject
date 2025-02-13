#include <iostream>
#include <fstream>
#include <map>
#include <nlohmann/json.hpp>
#include "TemperatureSensor.h"
#include "QueueManager.h"
#include "InfoNode.h"

using namespace std;
using json = nlohmann::json;
int main(){
    // Open a file to store temperature data
    ofstream output_file("temperature_data.json");
    if (!output_file.is_open()) {
        cerr << "Error: Unable to open file for writing." << endl;
        return EXIT_FAILURE;
    }
    // JSON array to store multiple readings
    json json_array = json::array();

    // Map sensor IDs to their corresponding directory names
    map<int, string> sensorMap {
            {1, "28-00000087fb7c"},
            {2, "28-00000085e6ff"},
            {3, "28-000000849be2"}
    };

    QueueManager queue_manager;

    TemperatureSensor temp_sensor1("sensor1", sensorMap[1],  queue_manager, 3);
    TemperatureSensor temp_sensor2("sensor2", sensorMap[2], queue_manager, 3);
    TemperatureSensor temp_sensor3("sensor2", sensorMap[3], queue_manager, 3);

    temp_sensor1.start_temp_reading_thread();
    temp_sensor2.start_temp_reading_thread();
    temp_sensor3.start_temp_reading_thread();
    this_thread::sleep_for(chrono::seconds(20));

    temp_sensor1.stop_temp_reading_thread();
    temp_sensor2.stop_temp_reading_thread();
    temp_sensor3.stop_temp_reading_thread();

    // Process stored temperature readings from the queue
    while (!queue_manager.is_empty()) {
        InfoNode node = queue_manager.pop_data();

        // Convert InfoNode data to JSON
        json sensor_json = {
                {"sensor", node.name},
                {"timestamp", node.timestamp},
                {"temperature", node.temperature}
        };

        // Print JSON to console
        cout << sensor_json.dump(4) << endl;
        // Add JSON object to array
        json_array.push_back(sensor_json);
    }
    // Write the JSON array to the file
    output_file << json_array.dump(4) << endl;
    output_file.close();

    return EXIT_SUCCESS;
}