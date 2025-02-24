//
// Created by andig on 21/02/2025.
//

#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <vector>
#include <string>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "DatabaseStorage.h"
#include "SensorManager.h"
#include "QueueManager.h"
#include "Observer.h"
#include "TemperatureStatistics.h"

class GUIManager {
private:
    GLFWwindow* window;
    const char* glsl_version;
    DatabaseStorage& database;
    SensorManager& sensorManager;
    QueueManager& queueManager;
    mutex& nodeDataMutex;
    UIObserver uiObserver;
    TemperatureStatistics& tempStats;
    bool isMeasuring;
    bool showGraph;

    // Real-time temperature values observed by uiObserver
    map<string, float>temp_map;

    // Graph data
    std::vector<float> time1, values1, time2, values2, time3, values3;

    // Helper functions
    void renderControls();
    void renderPlots();
    void renderRealTimeValues();
    void updatePlotData();
    void displayStatistics();

public:
    GUIManager(DatabaseStorage& db, SensorManager& sm, QueueManager& qm, map<string, float>& data_map,
               mutex& data_mutex, TemperatureStatistics& tempStats);
    ~GUIManager();

    void initialize_gui();            // Initialize the GUI
    void handleStartMeasurement();    // Start collecting sensor measurements
    void handleStopMeasurement();     // Stop collecting sensor measurements and generates graphs
    void render();                    // Render the GUI every frame
    void cleanup_gui();               // Cleanup ImGui and other resources
    GLFWwindow* getWindow() const;
};

#endif // GUIMANAGER_H
