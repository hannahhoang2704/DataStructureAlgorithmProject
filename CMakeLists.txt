cmake_minimum_required(VERSION 3.25)
project(DataStructureAlgorithmsProject)

set(CMAKE_CXX_STANDARD 17)
find_package(nlohmann_json REQUIRED)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

# Define ImGui source files
set(IMGUI_SRC
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/backends/imgui_impl_sdl2.cpp
        imgui/backends/imgui_impl_opengl3.cpp
        imgui/backends/imgui_impl_glfw.cpp
)



# Define ImGui header files (important to be explicitly listed)
set(IMGUI_HEADERS
        imgui/imgui.h
        imgui/imconfig.h
        imgui/imstb_textedit.h
        imgui/imstb_rectpack.h
        imgui/imstb_truetype.h
        imgui/imgui_internal.h
        imgui/backends/imgui_impl_sdl2.h
        imgui/backends/imgui_impl_glfw.h
        imgui/backends/imgui_impl_opengl3.h
)

set(IMGUI_SRC
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_demo.cpp
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_opengl3.cpp
)

# Create ImGui as a static library
add_library(imgui STATIC ${IMGUI_SRC})

# Include directories for ImGui
target_include_directories(imgui PUBLIC imgui imgui/backends ${SDL2_INCLUDE_DIRS})

# Link SDL2 and OpenGL to ImGui
target_link_libraries(imgui PUBLIC SDL2::SDL2 OpenGL::GL GLEW::GLEW)

add_executable(DataStructureAlgorithmsProject
        main.cpp
        TemperatureSensor.cpp
        TemperatureSensor.h
        InfoNode.h
        QueueManager.h
        QueueManager.cpp
        DatabaseStorage.cpp
        DatabaseStorage.h
        SensorManager.h
        SensorManager.cpp
        Observer.h
        GUIManager.h
        GUIManager.cpp
        LinearRegression.cpp
        LinearRegression.h
        Statistic.h
        Statistic.cpp
)

target_include_directories(DataStructureAlgorithmsProject PRIVATE /usr/local/include)
target_link_libraries(DataStructureAlgorithmsProject PRIVATE
        nlohmann_json::nlohmann_json
        imgui
        glfw
        SDL2::SDL2
        OpenGL::GL
        GLEW::GLEW
)



