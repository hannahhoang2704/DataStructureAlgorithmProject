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
    map<string, vector<float>> sensorTemperatures;       // Stores temperature values for each sensor
    map<string, vector<uint64_t>> sensorTimestamps;      // Stores timestamps for each sensor
    DatabaseStorage* databaseStorage;

public:
    TemperatureStatistics(DatabaseStorage* dbStorage);
    void addSensorData(const string& sensorName, const vector<float>& temps, const vector<uint64_t>& timestamps);
    void loadDataFromDatabase();
    pair<float, uint64_t> getMinTemperatureWithTimestamp(const string& sensorName) const;
    pair<float, uint64_t> getMaxTemperatureWithTimestamp(const string& sensorName) const;
    float getMinTemperature() const;
    float getMaxTemperature() const;
    float getAverageTemperature(const string& sensorName) const;
    float getAverageTemperatureAllSensors() const;
};

#endif // TEMPERATURESTATISTICS_H