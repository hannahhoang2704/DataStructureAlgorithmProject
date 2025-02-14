cmake_minimum_required(VERSION 3.25)
project(DataStructureAlgorithmsProject)

set(CMAKE_CXX_STANDARD 17)
find_package(nlohmann_json REQUIRED)

add_executable(DataStructureAlgorithmsProject
        main.cpp
        TemperatureSensor.cpp
        TemperatureSensor.h
        InfoNode.h
        QueueManager.h
        QueueManager.cpp
        DatabaseStorage.cpp
        DatabaseStorage.h)

target_include_directories(DataStructureAlgorithmsProject PRIVATE /usr/local/include)
target_link_libraries(DataStructureAlgorithmsProject PRIVATE nlohmann_json::nlohmann_json)
