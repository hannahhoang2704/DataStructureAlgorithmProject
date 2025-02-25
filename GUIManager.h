//
// Created by andig on 21/02/2025.
//

#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <vector>
#include <string>
#include <vector>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "DatabaseStorage.h"
#include "SensorManager.h"
#include "QueueManager.h"
#include "Observer.h"
#include "Statistic.h"
#include "InfoNode.h"

class GUIManager {
private:
    GLFWwindow* window;               // Pointer to the GLFW window
    const char* glsl_version;         // GLSL version string
    DatabaseStorage& database;        // Reference to the database
    SensorManager& sensorManager;     // Reference to the sensor manager
    QueueManager& queueManager;       // Reference to the queue manager
    mutex& nodeDataMutex;              // Mutex to ensure thread-safe access
    UIObserver uiObserver;            // Observer to track real-time sensor updates
    Statistic& statistics;

    bool isMeasuring;                 // Indicator if measurement is running
    bool showGraph;                   // Indicator to show graph after stopping

    // Real-time temperature values updated from a map of temperature name as key and temperature value (Observed by `uiObserver`)
    map<string, float>temp_map;
    vector<SensorInfo> &sensor_info;
    // Graph data
    std::vector<float> time1, values1, time2, values2, time3, values3;

    // Helper functions
    void renderControls();            // Render Start/Stop control buttons
    void renderPlots();               // Render the plots when showing the graph
    void renderRealTimeValues();      // Render real-time temperature values
    void updatePlotData();            // Update plot data upon stopping measurements
    void display_predict_temp();

public:
    GUIManager(DatabaseStorage& database, SensorManager& sensorManager, QueueManager& queueManager, Statistic& statistic, map<string, float>& data_map, mutex& data_mutex, vector<SensorInfo>&sensor_info
    );
    ~GUIManager();

    void initialize_gui();            // Initialize the GUI
    void handleStartMeasurement();    // Start collecting sensor measurements
    void handleStopMeasurement();     // Stop collecting sensor measurements and generates graphs
    void render();                    // Render the GUI every frame
    void cleanup_gui();               // Cleanup ImGui and other resources
    GLFWwindow* getWindow() const;
};

#endif // GUIMANAGER_H
