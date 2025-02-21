//
// Created by andig on 21/02/2025.
//

#include "GUIManager.h"
#include <iostream>

GUIManager::GUIManager(DatabaseStorage& db, SensorManager& sm, QueueManager& qm, float& t1, float& t2, float& t3)
        : database(db), sensorManager(sm), queueManager(qm),
          uiObserver(t1, t2, t3), // Initialize UIObserver
          temp1(t1), temp2(t2), temp3(t3),
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

    // Initialize ImPlot context
    ImPlot::CreateContext();
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
    ImGui::Begin("Measurement Controls");

    if (!isMeasuring) {
        if (ImGui::Button("Start Measurement")) {
            handleStartMeasurement();
        }
    } else {
        if (ImGui::Button("Stop Measurement")) {
            handleStopMeasurement();
        }
    }

    ImGui::End();
}

void GUIManager::renderRealTimeValues() {
    ImGui::Begin("Real-Time Sensor Values");
    ImGui::Text("Sensor 1: %.2f °C", temp1);
    ImGui::Text("Sensor 2: %.2f °C", temp2);
    ImGui::Text("Sensor 3: %.2f °C", temp3);
    ImGui::End();
}

void GUIManager::renderPlots() {
    ImGui::Begin("Sensor Data");

    if (ImPlot::BeginPlot("Combined Graph", "Time", "Temperature (°C)", ImVec2(600, 300))) {
        if (!values1.empty()) ImPlot::PlotLine("Sensor 1", time1.data(), values1.data(), time1.size());
        if (!values2.empty()) ImPlot::PlotLine("Sensor 2", time2.data(), values2.data(), time2.size());
        if (!values3.empty()) ImPlot::PlotLine("Sensor 3", time3.data(), values3.data(), time3.size());
        ImPlot::EndPlot();
    }

    ImGui::End();
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

void GUIManager::cleanup_gui() {
    ImPlot::DestroyContext();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}