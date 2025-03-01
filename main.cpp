
#include <vector>
#include "QueueManager.h"
#include "DatabaseStorage.h"
#include "SensorManager.h"
#include "GUIManager.h"
#include "TemperatureSensor.h"
#include "Statistics.h"
#include "LinearRegression.h"

using namespace std;

int main() {
    try {
        // Paths and initialization
        std::string json_file_path = "/home/andrea/Documents/projects/DataStructureAlgorithmProject/database/database.json"; // path on raspberry
        QueueManager queue_manager;
        DatabaseStorage database(json_file_path, queue_manager);
        SensorManager sensor_manager;
        LinearRegression linear_regression;
        Statistics statistics(linear_regression, database);

        std::vector<SensorInfo> sensors_config = {
                {"sensor1", "28-00000087fb7c", 2},
                {"sensor2", "28-00000085e6ff", 2},
                {"sensor3", "28-000000849be2", 4}
        };
        // Add sensors
        for (const auto& sensor : sensors_config) {
            sensor_manager.addSensor(new TemperatureSensor(sensor.name, sensor.fileName, queue_manager, sensor.interval));
        }

        map<string, float> sensors_data;
        mutex sensor_data_mutex;
    
        // Initialize GUIManager
        GUIManager gui_manager(database, sensor_manager, queue_manager, statistics, sensors_data, sensor_data_mutex, sensors_config);
        gui_manager.initialize_gui();

        // Main loop
        while (!glfwWindowShouldClose(gui_manager.getWindow())) {
            glfwPollEvents();
            gui_manager.render();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}