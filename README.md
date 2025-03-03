# Application Description

The application is a temperature measurement system using a Raspberry Pi 4 as the microcontroller. The system includes a graphical user interface (GUI) that displays the Raspberry Pi 4’s interface, allowing users to control the measurement process and gain valuable insights from the data.
The project is part of the "Data Structures and Algorithms Project" course.

The system comprises three waterproof temperature sensors Olimex DS18B20 connected directly to the Raspberry Pi 4 through GPIO pins. The Raspberry Pi acts as the microcontroller, reading temperature data from the sensors at set intervals, storing the data in a database, and providing a GUI for user interaction. Through the GUI, users can start and stop measurements, as well as visualize the temperature data in the form of graphs. The system performs basic data analysis, including calculations of the maximum, minimum, and average temperatures. To further enhance the system, a linear regression model is implemented to predict future temperature trends based on the latest historical data.

# Package Dependencies

This project requires the following dependencies to be installed. Follow the instructions below for your operating system.

## nlohmann/json
A JSON library for C++.

- **GitHub:** [nlohmann/json](https://github.com/nlohmann/json)
- **MacOS Installation:**
  ```sh
  brew install nlohmann-json
- **Ubuntu/Debian Installation:**
```sh
sudo apt install nlohmann-json3-dev
```

## imgui 
Prerequisites: to run imgui on raspbian, install the packages below:
```sh
sudo apt-get update
sudo apt install libglfw3-dev libgles2-mesa-dev
sudo apt install libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev
sudo apt install libglew-dev
```
# Compiling on Raspbian
```sh
mkdir build
cd build
cmake ..
make 
./DataStructureAlgorithmsProject
```
