cmake_minimum_required(VERSION 3.26)
project(DataStructureAlgorithmsProject)

set(CMAKE_CXX_STANDARD 17)

# Add the source directory
set(SOURCE_DIR ${CMAKE_SOURCE_DIR})

# Define the path to ImGui
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/libs/imgui)

# Add ImGui source files
set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
)

# Include the ImGui submodule
add_subdirectory(libs/imgui)

# Add SDL2 and OpenGL libraries
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

# Include the directories for ImGui, SDL2, and OpenGL
include_directories(${IMGUI_DIR})
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIR})

# Add source files for your project
add_executable(DataStructureAlgorithmsProject main.cpp ${IMGUI_SOURCES})

# Link the libraries to your project
target_link_libraries(DataStructureAlgorithmsProject
        ${SDL2_LIBRARIES}
        ${OPENGL_LIBRARIES}
)

# Set include directories
target_include_directories(DataStructureAlgorithmsProject
        imgui_impl_sdl2
        imgui_impl_opengl3
)