//
// Created by andig on 24/02/2025.
//

#ifndef TEMPERATURESTATISTICS_H
#define TEMPERATURESTATISTICS_H

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <numeric>
#include "DatabaseStorage.h"

using namespace std;

class TemperatureStatistics {
private:
    map<string, vector<float>> sensorTemperatures;
    map<string, vector<uint64_t>> sensorTimestamps;
    DatabaseStorage* databaseStorage;

public:
    TemperatureStatistics(DatabaseStorage* dbStorage);
    void addSensorData(const string& sensorName, const vector<float>& temps, const vector<uint64_t>& timestamps);
    void loadDataFromDatabase();
    void clearData();

    pair<float, string> getMinTemperatureWithTimestamp(const string& sensorName);
    pair<float, string> getMaxTemperatureWithTimestamp(const string& sensorName);
    float getMinTemperature();
    float getMaxTemperature();
    float getAverageTemperature(const string& sensorName);
    float getAverageTemperatureAllSensors();
};

#endif // TEMPERATURESTATISTICS_H