//
// Created by andig on 21/02/2025.
//

#include "GUIManager.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>

GUIManager::GUIManager(DatabaseStorage& db, SensorManager& sm, QueueManager& qm, Statistic& statistic,map<string, float>& data_map, mutex& data_mutex, vector<SensorInfo>&sensor_info, TemperatureStatistics& tempStats)
        : database(db), sensorManager(sm), queueManager(qm), statistics(statistic), temp_map(data_map), nodeDataMutex(data_mutex), sensor_info(sensor_info),
            uiObserver(temp_map, nodeDataMutex), tempStats(tempStats),
          isMeasuring(false), showGraph(false), window(nullptr), glsl_version(nullptr) {
    // Add the observer to the queue
    queueManager.add_observer(&uiObserver);

    // Initialize empty data for plots
    time1.clear();
    values1.clear();
    time2.clear();
    values2.clear();
    time3.clear();
    values3.clear();
}

GUIManager::~GUIManager() {
    cleanup_gui();
}

void GUIManager::initialize_gui() {
    // Set error callback
    glfwSetErrorCallback([](int error, const char* description) {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    });

    // Initialize GLFW
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    // Determine GLSL version
#if defined(IMGUI_IMPL_OPENGL_ES2)
    glsl_version = "#version 100";
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    glsl_version = "#version 300 es";
#elif defined(__APPLE__)
    glsl_version = "#version 150";
#else
    glsl_version = "#version 130";
#endif

    // Create GLFW window
    window = glfwCreateWindow(1280, 720, "Temperature Sensors GUI", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsLight();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void GUIManager::render() {
    // Start new ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Render real-time values if measuring
    if (isMeasuring) {
        renderRealTimeValues();
    }

    // Render Start/Stop controls
    renderControls();

    // Render plots after stopping measurements
    if (showGraph) {
        renderPlots();
        //Min, max, ave values and trend
        displayStatistics();
        display_predict_temp();
    }

    // Render GUI
    ImGui::Render();
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap buffers
    glfwSwapBuffers(window);
}

void GUIManager::renderControls() {

    if (!isMeasuring) {
        if (ImGui::Button("Start Measurement")) {
            handleStartMeasurement();
        }
    } else {
        if (ImGui::Button("Stop Measurement")) {
            handleStopMeasurement();
        }
    }
}

void GUIManager::renderRealTimeValues() {
    lock_guard<mutex> lock(nodeDataMutex); // Lock for thread safety

    for (const auto& entry : temp_map) {
        ImGui::Text("%s: %.2f °C", entry.first.c_str(), entry.second);
    }
}


void GUIManager::renderPlots() {

    // Plot Sensor 1
    if (!values1.empty()) {
        float min1 = *std::min_element(values1.begin(), values1.end());
        float max1 = *std::max_element(values1.begin(), values1.end());
        max1 += 1.0f;  // Add a small buffer to make the graph more readable
        min1 -= 1.0f;

        ImGui::Text("Sensor 1");
        ImGui::PlotLines("Temp 1", values1.data(), static_cast<int>(values1.size()), 0, nullptr, min1, max1, ImVec2(0, 200));

        ImGui::Spacing();
        auto [sensor1Min, min1Timestamp] = tempStats.getMinTemperatureWithTimestamp("sensor1");
        auto [sensor1Max, max1Timestamp] = tempStats.getMaxTemperatureWithTimestamp("sensor1");
        float sensor1Ave = tempStats.getAverageTemperature("sensor1");

        std::string min1TimestampStr = min1Timestamp;
        std::string max1TimestampStr = max1Timestamp;

        ImGui::Text("  Sensor1 Min: %.2f °C (At %s) | Sensor1 Max: %.2f °C (At %s) | Sensor1 Avg: %.2f °C",
                    sensor1Min, min1TimestampStr.c_str(), sensor1Max, max1TimestampStr.c_str(), sensor1Ave);
        ImGui::Spacing();
        ImGui::Spacing();
    }

    // Plot Sensor 2
    if (!values2.empty()) {
        float min2 = *std::min_element(values2.begin(), values2.end());
        float max2 = *std::max_element(values2.begin(), values2.end());
        max2 += 1.0f;
        min2 -= 1.0f;

        ImGui::Text("Sensor 2");
        ImGui::PlotLines("Temp 2", values2.data(), static_cast<int>(values2.size()), 0, nullptr, min2, max2, ImVec2(0, 200));

        ImGui::Spacing();
        auto [sensor2Min, min2Timestamp] = tempStats.getMinTemperatureWithTimestamp("sensor2");
        auto [sensor2Max, max2Timestamp] = tempStats.getMaxTemperatureWithTimestamp("sensor2");
        float sensor2Ave = tempStats.getAverageTemperature("sensor2");

        std::string min2TimestampStr = min2Timestamp;
        std::string max2TimestampStr = max2Timestamp;

        ImGui::Text("  Sensor2 Min: %.2f °C (At %s) | Sensor2 Max: %.2f °C (At %s) | Sensor2 Avg: %.2f °C",
                    sensor2Min, min2TimestampStr.c_str(), sensor2Max, min2TimestampStr.c_str(), sensor2Ave);
        ImGui::Spacing();
        ImGui::Spacing();
    }

    // Plot Sensor 3
    if (!values3.empty()) {
        float min3 = *std::min_element(values3.begin(), values3.end());
        float max3 = *std::max_element(values3.begin(), values3.end());
        max3 += 1.0f;
        min3 -= 1.0f;

        ImGui::Text("Sensor 3");
        ImGui::PlotLines("Temp 3", values3.data(), static_cast<int>(values3.size()), 0, nullptr, min3, max3, ImVec2(0, 200));

        ImGui::Spacing();
        auto [sensor3Min, min3Timestamp] = tempStats.getMinTemperatureWithTimestamp("sensor3");
        auto [sensor3Max, max3Timestamp] = tempStats.getMaxTemperatureWithTimestamp("sensor3");
        float sensor3Ave = tempStats.getAverageTemperature("sensor3");

        std::string min3TimestampStr = min3Timestamp;
        std::string max3TimestampStr = max3Timestamp;

        ImGui::Text("  Sensor3 Min: %.2f °C (At %s) | Sensor3 Max: %.2f °C (At %s) | Sensor3 Avg: %.2f °C",
                    sensor3Min, min3TimestampStr.c_str(), sensor3Max, min3TimestampStr.c_str(), sensor3Ave);
        ImGui::Spacing();
        ImGui::Spacing();
    }
}


void GUIManager::handleStartMeasurement() {
    // Start measurements
    sensorManager.startAll();
    database.start_write_thread();

    // Update state
    isMeasuring = true;
    showGraph = false;
}

void GUIManager::handleStopMeasurement() {
    // Stop measurements
    sensorManager.stopAll();
    database.stop_write_thread();

    // Update plot data and state
    updatePlotData();
    isMeasuring = false;
    showGraph = true;
}

void GUIManager::updatePlotData() {
    auto [timestamps, values] = database.read_database();

    // Prepare data for plotting
    database.preparePlotData("sensor1", timestamps, values, time1, values1);
    database.preparePlotData("sensor2", timestamps, values, time2, values2);
    database.preparePlotData("sensor3", timestamps, values, time3, values3);
}

void GUIManager::displayStatistics() {
    ImGui::Separator();
    float globalMin = tempStats.getMinTemperature();
    float globalMax = tempStats.getMaxTemperature();
    float globalAvg = tempStats.getAverageTemperatureAllSensors();

    // Display the summary
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Text("Summary of All Sensors:");
    ImGui::Text("  Minimum Temperature: %.2f °C", globalMin);
    ImGui::Text("  Maximum Temperature: %.2f °C", globalMax);
    ImGui::Text("  Average Temperature: %.2f °C", globalAvg);
    ImGui::Spacing();
    ImGui::Spacing();
}

void GUIManager::display_predict_temp() {
    for(auto &sensor: sensor_info){
        float predict_value=-100;
        statistics.predict_future_temp(sensor.name, static_cast<uint64_t>(sensor.interval), predict_value);
        ImGui::Text("Prediction: %s: %.2f °C ", sensor.name.c_str(), predict_value);
    }
}

void GUIManager::cleanup_gui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (window) {
        glfwDestroyWindow(window);
        glfwTerminate();
        window = nullptr;
    }
}

GLFWwindow* GUIManager::getWindow() const {
    return window;
}

