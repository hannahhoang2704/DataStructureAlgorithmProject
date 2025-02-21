//
// Created by andig on 21/02/2025.
//

#include "GUIManager.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <iostream>
#include <algorithm>

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
}

void GUIManager::render() {
    // Start a new ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Render everything in a single window
    ImGui::Begin("Temperature Sensors GUI"); // Start main GUI window

    // Render controls (start/stop measurement button)
    if (!isMeasuring) {
        if (ImGui::Button("Start Measurement")) {
            handleStartMeasurement();
        }
    } else {
        if (ImGui::Button("Stop Measurement")) {
            handleStopMeasurement();
        }
    }

    ImGui::Separator();

    // Render real-time sensor values
    ImGui::Text("Real-Time Sensor Values:");
    ImGui::Text("Sensor 1: %.2f °C", temp1);
    ImGui::Text("Sensor 2: %.2f °C", temp2);
    ImGui::Text("Sensor 3: %.2f °C", temp3);

    ImGui::Separator();

    // Render plots if the graph should be shown
    if (showGraph) {
        ImGui::Text("Sensor Data Plots:");

        // Sensor 1 Plot
        if (!values1.empty()) {
            float min1 = *std::min_element(values1.begin(), values1.end());
            float max1 = *std::max_element(values1.begin(), values1.end());
            max1 += 1.0f; // Add a small buffer to the max
            min1 -= 1.0f; // Add a small buffer to the min

            ImGui::Text("Sensor 1");
            ImGui::PlotLines("##Sensor1", values1.data(), static_cast<int>(values1.size()), 0, nullptr, min1, max1, ImVec2(0, 150));
        }

        // Sensor 2 Plot
        if (!values2.empty()) {
            float min2 = *std::min_element(values2.begin(), values2.end());
            float max2 = *std::max_element(values2.begin(), values2.end());
            max2 += 1.0f;
            min2 -= 1.0f;

            ImGui::Text("Sensor 2");
            ImGui::PlotLines("##Sensor2", values2.data(), static_cast<int>(values2.size()), 0, nullptr, min2, max2, ImVec2(0, 150));
        }

        // Sensor 3 Plot
        if (!values3.empty()) {
            float min3 = *std::min_element(values3.begin(), values3.end());
            float max3 = *std::max_element(values3.begin(), values3.end());
            max3 += 1.0f;
            min3 -= 1.0f;

            ImGui::Text("Sensor 3");
            ImGui::PlotLines("##Sensor3", values3.data(), static_cast<int>(values3.size()), 0, nullptr, min3, max3, ImVec2(0, 150));
        }
    }

    ImGui::End(); // End the main GUI window

    // Render and display the ImGui content
    ImGui::Render();
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap buffers
    glfwSwapBuffers(window);
}

void GUIManager::cleanup_gui() {
    // Clean up ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Clean up GLFW
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

void GUIManager::handleStartMeasurement() {
    // Start the measurement process
    isMeasuring = true;
    showGraph = false;

    database.start_write_thread();
    sensorManager.startAll();
}

void GUIManager::handleStopMeasurement() {
    // Stop the measurement process
    isMeasuring = false;
    showGraph = true;

    database.stop_write_thread();
    sensorManager.stopAll();
}

GLFWwindow* GUIManager::getWindow() const {
    return window; // Return the GLFW window pointer
}
