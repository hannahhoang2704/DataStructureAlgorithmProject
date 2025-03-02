//
// Created by Hanh Hoang on 25.2.2025.
//

#include "Statistics.h"
#define REGRESSION_DATA_SIZE 5

bool Statistics::predict_future_temp(const std::string &sensor, uint64_t interval, float& predict_temp_val) {
    auto [timestamps, temp_val] = db_reader.process_data();
    if(timestamps.find(sensor) == timestamps.end()){
        cerr << "Sensor not found" << endl;
        return false;
    }
    const auto& sensor_timestamps = timestamps[sensor];
    const auto& sensor_values = temp_val[sensor];
    auto data_size = sensor_timestamps.size();
    if(data_size < REGRESSION_DATA_SIZE){
        cerr << "Not enough data to give prediction with " << data_size << " data stored in database" << endl;
        return false;
    }
    cout << "add value for train linear regression: " << sensor << " ";
    for(size_t i = data_size - REGRESSION_DATA_SIZE; i < data_size; i++){
        cout << sensor_values[i] << " " << "[" << sensor_timestamps[i] << "]   ";
        linear_regression.addData(sensor_timestamps[i], sensor_values[i], sensor_timestamps[data_size-REGRESSION_DATA_SIZE]);
    }
    cout << endl;
    auto future_timestamp = sensor_timestamps[data_size-1] + interval;
    cout << "future timestamp " << future_timestamp << " " << "last timestamp " << sensor_timestamps[data_size-1];
    if(!linear_regression.trainModel()){
        return false;
    }
    predict_temp_val = linear_regression.predict_future(future_timestamp, sensor_timestamps[data_size-5]);
    cout << " start timestamp " << sensor_timestamps[data_size -5] << endl;
    return true;
}

void Statistics::preparePlotData(
        const std::string &sensorName,
        const map<std::string, vector<uint64_t>> &timestamps,
        const map<std::string, vector<float>> &values,
        vector<float> &timeInSeconds,
        vector<float> &sensorValues) {
    timeInSeconds.clear();
    sensorValues.clear();

    auto timeIter = timestamps.find(sensorName);
    auto valueIter = values.find(sensorName);

    if (timeIter == timestamps.end() || valueIter == values.end()) {
        cerr << "Sensor data not found for sensor: " << sensorName << endl;
        return;
    }

    const auto& rawTimestamps = timeIter->second;
    const auto& rawValues = valueIter->second;

    if (rawTimestamps.size() != rawValues.size()) {
        cerr << "Mismatch in timestamps and values size for sensor: " << sensorName << endl;
        return;
    }

    uint64_t startTime = rawTimestamps.front();
    for (size_t i = 0; i < rawTimestamps.size(); ++i) {
        float timeInSecondsValue = (rawTimestamps[i] - startTime) / 1000.0f; // Convert to seconds
        timeInSeconds.push_back(timeInSecondsValue);
        sensorValues.push_back(rawValues[i]);
    }
    cout << "Prepared data for sensor: " << sensorName << endl;
}
void Statistics::addSensorData(const string& sensorName, const vector<float>& temps, const vector<uint64_t>& timestamps) {
    if (temps.size() != timestamps.size()) {
        throw runtime_error("Mismatch between temperature values and timestamps for sensor: " + sensorName);
    }
    // Print the temperature and timestamp data for debugging
//    std::cout << "Adding data for sensor: " << sensorName << std::endl;
//    std::cout << "Temperatures: ";
//    for (const auto& temp : temps) {
//        std::cout << temp << " ";
//    }
//    std::cout << std::endl;
//
//    std::cout << "Timestamps: ";
//    for (const auto& timestamp : timestamps) {
//        std::cout << timestamp << " ";
//    }
//    std::cout << std::endl;
    sensorTemperatures[sensorName] = temps;
    sensorTimestamps[sensorName] = timestamps;
}


void Statistics::loadDataFromDatabase() {
//    if (!sensorTemperatures.empty() && !sensorTimestamps.empty()) {
//        return;
//    }

    auto [timestamps, values] = db_reader.process_data();

    clearData();
    for (const auto& [sensorName, temps] : values) {
        addSensorData(sensorName, temps, timestamps[sensorName]);
    }
//    std::cout << "Loaded data for " << values.size() << " sensors from the database." << std::endl;
}

pair<float, string> Statistics::getMinTemperatureWithTimestamp(const string& sensorName) {
//    loadDataFromDatabase();

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

pair<float, string> Statistics::getMaxTemperatureWithTimestamp(const string& sensorName) {
//    loadDataFromDatabase();

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

float Statistics::getMinTemperature() {
//    loadDataFromDatabase();

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

float Statistics::getMaxTemperature() {
//    loadDataFromDatabase();

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

float Statistics::getAverageTemperature(const string& sensorName) {
//    loadDataFromDatabase();

    auto sensorIt = sensorTemperatures.find(sensorName);
    if (sensorIt == sensorTemperatures.end() || sensorIt->second.empty()) {
        throw runtime_error("Sensor '" + sensorName + "' not found!");
    }

    const auto& temps = sensorIt->second;
    float sum = accumulate(temps.begin(), temps.end(), 0.0f);
    return sum / temps.size();
}

float Statistics::getAverageTemperatureAllSensors() {
//    loadDataFromDatabase();

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

std::string Statistics::formatTimestamp(uint64_t rawTimestamp) {
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

void Statistics::clearData() {
    sensorTemperatures.clear();
    sensorTimestamps.clear();
}