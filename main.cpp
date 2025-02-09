#include <iostream>
#include <fstream>
#include <map>
#include <queue>
#include <unistd.h>  // For sleep()
#include <vector>

using namespace std;

#define SENSOR_ID "28-00000087fb7c"
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
        {1, "28-00000087fb7c"},
        {2, "28-00000085e6ff"}
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

void printQueue() {
    queue<TempReading> tempCopy = tempQueue;
    while (!tempCopy.empty()) {
        TempReading reading = tempCopy.front();
        cout << "Sensor " << reading.sensorID << ": " << reading.value << "°C at  " << ctime(&reading.timestamp);
        tempCopy.pop();
    }
}

int main() {
    while(true) {
        // Get temperature data from sensors
        for (const auto& [id, sensorDir] : sensorMap) {
            double temp = get_temp(sensorDir);
            addTempToQueue(temp, id);
        }
        printQueue();
        sleep(2);
    }
}
