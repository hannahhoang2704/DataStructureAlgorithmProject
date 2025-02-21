//
// Created by andig on 21/02/2025.
//

#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <vector>
#include <string>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <implot.h>
#include "DatabaseStorage.h"
#include "SensorManager.h"
#include "QueueManager.h"
#include "Observer.h"

class GUIManager {
private:
    GLFWwindow* window;               // Pointer to the GLFW window
    const char* glsl_version;         // GLSL version string
    DatabaseStorage& database;        // Reference to the database
    SensorManager& sensorManager;     // Reference to the sensor manager
    QueueManager& queueManager;       // Reference to the queue manager

    UIObserver uiObserver;            // Observer to track real-time sensor updates

    bool isMeasuring;                 // Indicator if measurement is running
    bool showGraph;                   // Indicator to show graph after stopping

    // Real-time temperature values (Observed by `uiObserver`)
    float& temp1;
    float& temp2;
    float& temp3;

    // Graph data
    std::vector<float> time1, values1, time2, values2, time3, values3;

    // Helper functions
    void renderControls();            // Render Start/Stop control buttons
    void renderPlots();               // Render the plots when showing the graph
    void renderRealTimeValues();      // Render real-time temperature values
    void updatePlotData();            // Update plot data upon stopping measurements

public:
    GUIManager(DatabaseStorage& database, SensorManager& sensorManager, QueueManager& queueManager, float& t1, float& t2, float& t3);
    ~GUIManager();

    void initialize_gui();            // Initialize the GUI
    void handleStartMeasurement();    // Start collecting sensor measurements
    void handleStopMeasurement();     // Stop collecting sensor measurements and generates graphs
    void render();                    // Render the GUI every frame
    void cleanup_gui();               // Cleanup ImGui and other resources
};

#endif // GUIMANAGER_H
