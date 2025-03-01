//
// Created by andig on 24/02/2025.
//

#include "TemperatureStatistics.h"
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <ctime>


TemperatureStatistics::TemperatureStatistics(DatabaseStorage* dbStorage) : databaseStorage(dbStorage) {}

void TemperatureStatistics::addSensorData(const string& sensorName, const vector<float>& temps, const vector<uint64_t>& timestamps) {
    if (temps.size() != timestamps.size()) {
        throw runtime_error("Mismatch between temperature values and timestamps for sensor: " + sensorName);
    }
    sensorTemperatures[sensorName] = temps;
    sensorTimestamps[sensorName] = timestamps;
}

void TemperatureStatistics::loadDataFromDatabase() {
    if (!databaseStorage) {
        throw std::runtime_error("DatabaseStorage instance is not initialized");
    }

    if (!sensorTemperatures.empty() && !sensorTimestamps.empty()) {
        return;
    }

    auto jsonData = databaseStorage->read_database();

    clearData();

    for (const auto& sensorEntry : jsonData.items()) {
        const std::string& sensorName = sensorEntry.key();           // Sensor name
        const auto& data = sensorEntry.value();                      // Map of {timestamp: value}

        std::vector<uint64_t> timestamps;
        std::vector<float> values;

        for (const auto& entry : data.items()) {
            uint64_t timestamp = std::stoull(entry.key());           // Extract timestamp
            float value = entry.value().get<float>();                // Extract temperature value
            timestamps.push_back(timestamp);
            values.push_back(value);
        }

        addSensorData(sensorName, values, timestamps);
    }
}

pair<float, string> TemperatureStatistics::getMinTemperatureWithTimestamp(const string& sensorName) {
    loadDataFromDatabase();

    auto sensorIt = sensorTemperatures.find(sensorName);
    if (sensorIt == sensorTemperatures.end() || sensorIt->second.empty()) {
        throw runtime_error("Sensor '" + sensorName + "' not found or has no data!");
    }

    const auto& temps = sensorIt->second;
    const auto& timestamps = sensorTimestamps.at(sensorName);
    auto minIter = min_element(temps.begin(), temps.end());
    if (minIter == temps.end()) {
        throw runtime_error("No data available to find minimum temperature for sensor: " + sensorName);
    }
    size_t minIndex = distance(temps.begin(), minIter);

    uint64_t rawTimestamp = timestamps[minIndex];

    return {*minIter, formatTimestamp(rawTimestamp)};
}


pair<float, string> TemperatureStatistics::getMaxTemperatureWithTimestamp(const string& sensorName) {
    loadDataFromDatabase();

    auto sensorIt = sensorTemperatures.find(sensorName);
    if (sensorIt == sensorTemperatures.end() || sensorIt->second.empty()) {
        throw runtime_error("Sensor '" + sensorName + "' not found or has no data!");
    }

    const auto& temps = sensorIt->second;
    const auto& timestamps = sensorTimestamps.at(sensorName);
    auto maxIter = max_element(temps.begin(), temps.end());
    if (maxIter == temps.end()) {
        throw runtime_error("No data available to find maximum temperature for sensor: " + sensorName);
    }
    size_t maxIndex = distance(temps.begin(), maxIter);

    uint64_t rawTimestamp = timestamps[maxIndex];

    return {*maxIter, formatTimestamp(rawTimestamp)};
}


float TemperatureStatistics::getMinTemperature() {
    loadDataFromDatabase();

    if (sensorTemperatures.empty()) {
        throw runtime_error("No temperature data available.");
    }

    float globalMin = numeric_limits<float>::max();
    // Iterate through the latest data from `sensorTemperatures`
    for (const auto& [sensorName, temps] : sensorTemperatures) {
        float minTemp = *min_element(temps.begin(), temps.end());
        globalMin = min(globalMin, minTemp);
    }
    return globalMin;
}

float TemperatureStatistics::getMaxTemperature() {
    loadDataFromDatabase();

    if (sensorTemperatures.empty()) {
        throw runtime_error("No temperature data available.");
    }

    float globalMax = numeric_limits<float>::lowest();
    // Iterate through the latest data from `sensorTemperatures`
    for (const auto& [sensorName, temps] : sensorTemperatures) {
        float maxTemp = *max_element(temps.begin(), temps.end());
        globalMax = max(globalMax, maxTemp);
    }
    return globalMax;
}

float TemperatureStatistics::getAverageTemperature(const string& sensorName) {
    loadDataFromDatabase();

    auto sensorIt = sensorTemperatures.find(sensorName);
    if (sensorIt == sensorTemperatures.end() || sensorIt->second.empty()) {
        throw runtime_error("Sensor '" + sensorName + "' not found!");
    }

    const auto& temps = sensorIt->second;
    float sum = accumulate(temps.begin(), temps.end(), 0.0f);
    return sum / temps.size();
}

float TemperatureStatistics::getAverageTemperatureAllSensors() {
    loadDataFromDatabase();

    if (sensorTemperatures.empty()) {
        throw runtime_error("No temperature data available.");
    }

    float totalSum = 0.0;
    size_t totalCount = 0;

    for (const auto& [sensorName, temps] : sensorTemperatures) {
        totalSum += std::accumulate(temps.begin(), temps.end(), 0.0);
        totalCount += temps.size();
    }

    if (totalCount == 0) {
        throw runtime_error("No temperature readings available.");
    }

    return totalSum / totalCount;
}

std::string TemperatureStatistics::formatTimestamp(uint64_t rawTimestamp) {
    if (rawTimestamp == 0) {
        return "Invalid Timestamp";
    }

    std::time_t timeT = static_cast<std::time_t>(rawTimestamp);
    std::tm localTime;
    std::ostringstream oss;

    if (localtime_r(&timeT, &localTime)) {
        oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    return "Invalid Timestamp";
}



void TemperatureStatistics::clearData() {
    sensorTemperatures.clear();
    sensorTimestamps.clear();
}