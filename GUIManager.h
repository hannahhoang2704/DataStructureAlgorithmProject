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
#include "TemperatureStatistics.h"
#include "Statistic.h"
#include "InfoNode.h"

class GUIManager {
private:
    GLFWwindow* window;
    const char* glsl_version;
    DatabaseStorage& database;
    SensorManager& sensorManager;
    QueueManager& queueManager;
    mutex& nodeDataMutex;
    UIObserver uiObserver;
    Statistic& statistics;
    TemperatureStatistics& tempStats;
    bool isMeasuring;
    bool showStats;

    //map of temperature name as key and temperature value observed by uiObserver
    map<string, float>temp_map;
    vector<SensorInfo> &sensor_info;
    // Graph data
    std::vector<float> time1, values1, time2, values2, time3, values3;

    // Helper functions
    void renderControls();
    void renderPlots();
    void renderRealTimeValues();
    void updatePlotData();
    void display_predict_temp(string sensorName);
    void displayStatistics();

public:
    GUIManager(DatabaseStorage& database, SensorManager& sensorManager, QueueManager& queueManager, Statistic& statistic, map<string, float>& data_map, mutex& data_mutex, vector<SensorInfo>&sensor_info, TemperatureStatistics& tempStats
    );
    ~GUIManager();

    void initialize_gui();
    void handleStartMeasurement();
    void handleStopMeasurement();
    void render();
    void cleanup_gui();
    GLFWwindow* getWindow() const;
};

#endif // GUIMANAGER_H
