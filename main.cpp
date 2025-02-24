
#include "QueueManager.h"
#include "DatabaseStorage.h"
#include "SensorManager.h"
#include "GUIManager.h"
#include "TemperatureSensor.h"
#include "TemperatureStatistics.h"

int main() {
    try {
        // Paths and initialization
        std::string json_file_path = "/home/andrea/Documents/projects/DataStructureAlgorithmProject/database/database.json"; // path on raspberry
        QueueManager queue_manager;
        DatabaseStorage database(json_file_path, queue_manager);
        SensorManager sensor_manager;
        TemperatureStatistics tempStats(&database);

        // Add sensors
        sensor_manager.addSensor(new TemperatureSensor("sensor1", "28-00000087fb7c", queue_manager, 2));
        sensor_manager.addSensor(new TemperatureSensor("sensor2", "28-00000085e6ff", queue_manager, 3));
        sensor_manager.addSensor(new TemperatureSensor("sensor3", "28-000000849be2", queue_manager,2));
        map<string, float> sensors_data;
        mutex sensor_data_mutex;
    
        // Initialize GUIManager
        GUIManager gui_manager(database, sensor_manager, queue_manager, sensors_data, sensor_data_mutex, tempStats);
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