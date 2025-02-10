#include <iostream>
#include <nlohmann/json.hpp>
#include "TemperatureSensor.h"
#include "QueueManager.h"

using namespace std;
using json = nlohmann::json;
int main(){
    //test json
    json j = {{"sensor", "test_sensor"}, {"temperature", 30}};
    std::cout << j.dump(4) << std::endl;

    QueueManager queue_manager;
    TemperatureSensor temp_sensor1("sensor1", 0,  queue_manager, 3);
    TemperatureSensor temp_sensor2("sensor2",1, queue_manager);
    temp_sensor1.start_temp_reading_thread();
    temp_sensor2.start_temp_reading_thread();
    this_thread::sleep_for(chrono::seconds(20));

    temp_sensor1.stop_temp_reading_thread();
    temp_sensor2.stop_temp_reading_thread();
    return EXIT_SUCCESS;
}