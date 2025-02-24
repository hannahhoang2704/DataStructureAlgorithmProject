//
// Created by andig on 24/02/2025.
//

#include "TemperatureStatistics.h"
#include <stdexcept>

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
        throw runtime_error("DatabaseStorage instance is not initialized");
    }

    auto [timestamps, values] = databaseStorage->read_database();

    // Clear any existing data and populate new data
    clearData();
    for (const auto& [sensorName, temps] : values) {
        addSensorData(sensorName, temps, timestamps[sensorName]);
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
    std::time_t time = static_cast<std::time_t>(rawTimestamp);

    std::ostringstream formattedTime;
    std::tm* localTime = std::localtime(&time);
    if (localTime) {
        formattedTime << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
        return formattedTime.str();
    }
    return "Invalid Time";
}


void TemperatureStatistics::clearData() {
    sensorTemperatures.clear();
    sensorTimestamps.clear();
}