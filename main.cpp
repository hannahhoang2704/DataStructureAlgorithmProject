#include <iostream>
#include <nlohmann/json.hpp>

//#include <SDL2/SDL.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
//#include <GL/glew.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>

#include "TemperatureSensor.h"
#include "QueueManager.h"
#include "DatabaseStorage.h"
#include "InfoNode.h"

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

using namespace std;
using json = nlohmann::json;

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}
int main(){
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    string json_file_path = "/Users/HannahHoang/DataStructureAlgorithmProject/database/database.json"; //MUST CHANGE THIS TO LOCAL ABS PATH
//    string json_file_path = "/home/hannah/DataStructureAlgorithmProject/database/database.json"; //MUST CHANGE THIS TO LOCAL ABS PATH

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char* glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif


    QueueManager queue_manager;
    DatabaseStorage database(json_file_path, queue_manager);
//    database.start_write_thread();
    TemperatureSensor temp_sensor1("sensor1", 0,  queue_manager, 3);
    TemperatureSensor temp_sensor2("sensor2",1, queue_manager);
    TemperatureSensor temp_sensor3("sensor3",1, queue_manager, 4);
//    temp_sensor1.start_temp_reading_thread();
//    temp_sensor2.start_temp_reading_thread();
//    temp_sensor3.start_temp_reading_thread();

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);
    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
//    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window))
#endif
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
//        if (show_demo_window)
//            ImGui::ShowDemoWindow(&show_demo_window);

        static float temp_values1[10] = {0};
        static float temp_values2[10] = {0};
        static float temp_values3[10] = {0};
        static int temp_offset = 0;
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
//            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Sensor Dashboard");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("Sensor Controller");               // Display some text (you can use a format strings too)
//            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
//            ImGui::Checkbox("Another Window", &show_another_window);

            if (ImGui::Button("Start Measure")) {
                counter++;
                database.start_write_thread();
                temp_sensor1.start_temp_reading_thread();
                temp_sensor2.start_temp_reading_thread();
                temp_sensor3.start_temp_reading_thread();
            }                           // Buttons return true when clicked (most widgets return true when edited/activated)

            ImGui::SameLine();
//            ImGui::Text("Start measuring counter = %d", counter);
            if (ImGui::Button("Stop Measure"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            {
                counter--;
                temp_sensor1.stop_temp_reading_thread();
                temp_sensor2.stop_temp_reading_thread();
                temp_sensor3.stop_temp_reading_thread();
                database.stop_write_thread();
            }


            // Get latest temperature readings
            int temp1 = temp_sensor1.get_temperature();
            int temp2 = temp_sensor2.get_temperature();
            int temp3 = temp_sensor3.get_temperature();

            ImGui::Text("Sensor 1:  %d°C", temp1);
            ImGui::Text("Sensor 2:  %d°C", temp2);
            ImGui::Text("Sensor 3:  %d°C", temp3);

            // Update circular buffers with new readings
            temp_values1[temp_offset] = static_cast<float>(temp1);
            temp_values2[temp_offset] = static_cast<float>(temp2);
            temp_values3[temp_offset] = static_cast<float>(temp3);
            temp_offset = (temp_offset + 1) % IM_ARRAYSIZE(temp_values1);

            // Calculate average temperatures
            auto calc_avg = [](float *values) {
                return std::accumulate(values, values + IM_ARRAYSIZE(temp_values1), 0.0f) / IM_ARRAYSIZE(temp_values1);
            };

            // Plot Sensor 1 Graph
            char overlay1[32]; sprintf(overlay1, "Avg: %.2f°C", calc_avg(temp_values1));
            ImGui::PlotLines("Sensor 1 Temperature", temp_values1, IM_ARRAYSIZE(temp_values1), temp_offset, overlay1, 0.0f, 30.0f, ImVec2(0, 30.0f));

            // Plot Sensor 2 Graph
            char overlay2[32]; sprintf(overlay2, "Avg: %.2f°C", calc_avg(temp_values2));
            ImGui::PlotLines("Sensor 2 Temperature", temp_values2, IM_ARRAYSIZE(temp_values2), temp_offset, overlay2, 0.0f, 30.0f, ImVec2(0, 30.0f));

            // Plot Sensor 3 Graph
            char overlay3[32]; sprintf(overlay3, "Avg: %.2f°C", calc_avg(temp_values3));
            ImGui::PlotLines("Sensor 3 Temperature", temp_values3, IM_ARRAYSIZE(temp_values3), temp_offset, overlay3, 0.0f, 30.0f, ImVec2(0, 30.0f));

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w=100, display_h=100;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
//    this_thread::sleep_for(chrono::seconds(15));


//    temp_sensor1.stop_temp_reading_thread();
//    temp_sensor2.stop_temp_reading_thread();
//    temp_sensor3.stop_temp_reading_thread();
//    database.stop_write_thread();

//    json j_data;
//    j_data = database.read_database();
//    cout << "json database  " << j_data.dump(4) << endl;
//
//    //Do search and filter data from database json file
//    map<string, map<uint64_t, int>> sensor_data = j_data.get<map<string, map<uint64_t, int>>>();
//    for (const auto& [sensor, readings] : sensor_data) {
//        cout << "Sensor: " << sensor << endl;
//        for (const auto& [timestamp, temp] : readings) {
//            if (temp > 3) {
//                cout << "  Timestamp: " << timestamp << " -> Temp: " << temp << endl;
//            }
//        }
//    }
