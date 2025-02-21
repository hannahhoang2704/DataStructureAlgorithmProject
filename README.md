
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

## imgui dependencies
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
