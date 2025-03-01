//
// Created by andig on 21/02/2025.
//

#include "GUIManager.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>

GUIManager::GUIManager(DatabaseStorage& db, SensorManager& sm, QueueManager& qm, Statistics& statistic, map<string, float>& data_map, mutex& data_mutex, vector<SensorInfo>&sensor_info)
        : database(db), sensorManager(sm), queueManager(qm), statistics(statistic), temp_map(data_map), nodeDataMutex(data_mutex), sensor_info(sensor_info),
            uiObserver(temp_map, nodeDataMutex),
          isMeasuring(false), showStats(false), window(nullptr), glsl_version(nullptr) {
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
    // create a top-level window that fills the screen
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Main Content", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);


    // Render real-time values if measuring
    if (isMeasuring) {
        renderRealTimeValues();
    }

    // Render Start/Stop controls
    renderControls();

    // Render plots and statistics after stopping measurements
    if (showStats) {
        renderPlotsAndStats();
        displayStatistics();
    }

    ImGui::End();

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


void GUIManager::renderPlotsAndStats() {
    renderPlotAndStatsForSensor("sensor1", values1);
    renderPlotAndStatsForSensor("sensor2", values2);
    renderPlotAndStatsForSensor("sensor3", values3);
}

void GUIManager::renderPlotAndStatsForSensor(const string& sensorName, vector<float> &values){
    if (!values.empty()) {
        float min = *std::min_element(values.begin(), values.end());
        float max = *std::max_element(values.begin(), values.end());
        max += 1.0f;  // Add a small buffer to make the graph more readable
        min -= 1.0f;

        ImGui::Text("%s", sensorName.c_str());
        ImGui::PlotLines(("Temp " + sensorName).c_str(), values.data(), static_cast<int>(values.size()), 0, nullptr, min, max, ImVec2(0, 200));

        ImGui::Spacing();
        auto [sensorMin, minTimestamp] = statistics.getMinTemperatureWithTimestamp(sensorName);
        auto [sensorMax, maxTimestamp] = statistics.getMaxTemperatureWithTimestamp(sensorName);
        float sensorAvg = statistics.getAverageTemperature(sensorName);
        string minTimestampStr = minTimestamp;
        string maxTimestampStr = maxTimestamp;
        ImGui::Text("  %s Min: %.2f °C (At %s) | %s Max: %.2f °C (At %s) | %s Avg: %.2f °C",
                    sensorName.c_str(), sensorMin, minTimestampStr.c_str(), sensorName.c_str(), sensorMax, maxTimestampStr.c_str(), sensorName.c_str(), sensorAvg);
        ImGui::Spacing();

        float sensorPrediction = display_predict_temp(sensorName);
        if(sensorPrediction != -100){
            ImGui::Text(" %s prediction: %.2f °C", sensorName.c_str(), sensorPrediction);
        }

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Separator();
    }
}

void GUIManager::handleStartMeasurement() {
    // Start measurements
    sensorManager.startAll();
    database.start_write_thread();

    // Update state
    isMeasuring = true;
    showStats = false;
}

void GUIManager::handleStopMeasurement() {
    // Stop measurements
    sensorManager.stopAll();
    database.stop_write_thread();

    // Update plot data and state
    updatePlotData();
    isMeasuring = false;
    showStats = true;
}

void GUIManager::updatePlotData() {
    auto [timestamps, values] = database.process_data();

    // Prepare data for plotting
    statistics.preparePlotData("sensor1", timestamps, values, time1, values1);
    statistics.preparePlotData("sensor2", timestamps, values, time2, values2);
    statistics.preparePlotData("sensor3", timestamps, values, time3, values3);
}

void GUIManager::displayStatistics() {
    ImGui::Separator();
    float globalMin = statistics.getMinTemperature();
    float globalMax = statistics.getMaxTemperature();
    float globalAvg = statistics.getAverageTemperatureAllSensors();

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

float GUIManager::display_predict_temp(const string& sensorName) {
    for(auto &sensor: sensor_info){
        if(sensor.name == sensorName){
            float predict_value=-100;
            statistics.predict_future_temp(sensor.name, static_cast<uint64_t>(sensor.interval), predict_value);
            return predict_value;
        }
    }
    return -100;
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

