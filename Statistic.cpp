//
// Created by Hanh Hoang on 25.2.2025.
//

#include "Statistic.h"
#define REGRESSION_DATA_SIZE 5

bool Statistic::predict_future_temp(const std::string &sensor, uint64_t interval, float& predict_temp_val) {
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
    for(size_t i = data_size - REGRESSION_DATA_SIZE; i < data_size; i++){
        linear_regression.addData(sensor_timestamps[i], sensor_values[i], sensor_timestamps[data_size-REGRESSION_DATA_SIZE]);
    }
    auto future_timestamp = sensor_timestamps[data_size-1] + interval;
    if(!linear_regression.trainModel()){
        return false;
    }
    predict_temp_val = linear_regression.predict_future(future_timestamp, sensor_timestamps[data_size-5]);
    return true;
}

void Statistic::preparePlotData(
        const std::string &sensorName,
        const map<std::string, vector<uint64_t>> &timestamps,
        const map<std::string, vector<float>> &values,
        vector<float> &timeInSeconds,
        vector<float> &sensorValues
) {
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