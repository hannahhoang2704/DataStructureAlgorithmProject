#include <iostream>
#include <fstream>
#include <map>
#include <queue>
#include <unistd.h>  // For sleep()
#include <vector>
#include <imgui.h>
#include <SDL.h>
#include <GL/glew.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>

using namespace std;

#define SENSOR_ID "28-01193a10a833"
#define SENSORS_PATH "/sys/bus/w1/devices/"
#define MAX_QUEUE_SIZE 100

// Store temperature readings in a queue and map
struct TempReading {
    double value;
    time_t timestamp;
    int sensorID;

    TempReading(double temp, int id) : value(temp), sensorID(id) {
        timestamp = time(nullptr); // get current time
    }
};

queue<TempReading> tempQueue;
map<int, string> sensorMap {
        {1, "28-01193a10a833"}
};

vector<float> temperatureHistory;  // For storing temperature readings for the graph

// Function to read temperature data from a sensor file
float get_temp(const string& sensorDir) {
    ifstream file(SENSORS_PATH + sensorDir + "/w1_slave");
    if (!file.is_open()) {
        cerr << "Error: Could not open " << SENSORS_PATH + sensorDir << endl;
        return 0.0;
    }

    string line, temp_data;
    while (getline(file, line)) {
        if (line.find("t=") != string::npos) {
            temp_data = line.substr(line.find("t=") + 2);
        }
    }
    file.close();

    if (!temp_data.empty()) {
        return stoi(temp_data) / 1000.0;
    }

    return 0.0;
}

// Add temperature readings to the queue and history
void addTempToQueue(double temp, int sensorID) {
    if (tempQueue.size() >= MAX_QUEUE_SIZE) {
        tempQueue.pop();
    }
    tempQueue.push(TempReading(temp, sensorID));
    temperatureHistory.push_back(temp);

    // Limit the number of points on the graph (optional)
    if (temperatureHistory.size() > 100) {
        temperatureHistory.erase(temperatureHistory.begin());
    }
}

// Function to initialize SDL, OpenGL, and ImGui
void initImGui(SDL_Window** window, SDL_GLContext* gl_context) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
        cerr << "Error initializing SDL: " << SDL_GetError() << endl;
        exit(1);
    }

    *window = SDL_CreateWindow("Sensor Data Visualization", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!*window) {
        cerr << "Error creating SDL window: " << SDL_GetError() << endl;
        exit(1);
    }

    *gl_context = SDL_GL_CreateContext(*window);
    if (!*gl_context) {
        cerr << "Error creating OpenGL context: " << SDL_GetError() << endl;
        exit(1);
    }

    if (glewInit() != GLEW_OK) {
        cerr << "Error initializing GLEW!" << endl;
        exit(1);
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(*window, *gl_context);
    ImGui_ImplOpenGL3_Init("#version 130");
}

// Cleanup ImGui resources
void cleanupImGui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

// Main loop to handle sensor readings and display the graph
int main() {
    SDL_Window* window;
    SDL_GLContext gl_context;
    initImGui(&window, &gl_context);

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        // Get temperature data from sensors
        for (const auto& [id, sensorDir] : sensorMap) {
            double temp = get_temp(sensorDir);
            addTempToQueue(temp, id);
        }

        // Create a simple window for displaying the graph
        ImGui::Begin("Sensor Temperature Graph");

        // Plot the temperature history as a line graph
        ImGui::PlotLines("Temperature", temperatureHistory.data(), temperatureHistory.size(), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 80));

        ImGui::End();

        // Render ImGui frame
        ImGui::Render();
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);

        // Delay to simulate sensor data acquisition
        sleep(2);
    }

    cleanupImGui();
    SDL_DestroyWindow(window);
    SDL_GL_DeleteContext(gl_context);
    SDL_Quit();
}
