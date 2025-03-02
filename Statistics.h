//
// Created by Hanh Hoang on 25.2.2025.
//

#ifndef DATASTRUCTUREALGORITHMSPROJECT_STATISTICS_H
#define DATASTRUCTUREALGORITHMSPROJECT_STATISTICS_H
#include <iostream>
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <ctime>
#include "DatabaseStorage.h"
#include "LinearRegression.h"
#include "InfoNode.h"

using namespace std;
class Statistics {
private:
    LinearRegression& linear_regression;
    DatabaseStorage& db_reader;
    map<string, vector<float>> sensorTemperatures;
    map<string, vector<uint64_t>> sensorTimestamps;
    map<string, LinearRegression> sensorRegressions; // Separate regression for each sensor
    static string formatTimestamp(uint64_t rawTimestamp);

public:
    Statistics(LinearRegression &linear_reg, DatabaseStorage &db_reader): linear_regression(linear_reg), db_reader(db_reader){};
    bool predict_future_temp(const string& sensor, uint64_t interval, float& predict_temp_val);
    void preparePlotData(const string& sensorName,
                         const map<string, vector<uint64_t>>& timestamps,
                         const map<string, vector<float>>& values,
                         vector<float>& timeInSeconds, vector<float>& sensorValues);
    void loadDataFromDatabase();
    void clearData();
    void addSensorData(const string& sensorName, const vector<float>& temps, const vector<uint64_t>& timestamps);
    pair<float, string> getMinTemperatureWithTimestamp(const string& sensorName);
    pair<float, string> getMaxTemperatureWithTimestamp(const string& sensorName);
    float getMinTemperature();
    float getMaxTemperature();
    float getAverageTemperature(const string& sensorName);
    float getAverageTemperatureAllSensors();
};


#endif //DATASTRUCTUREALGORITHMSPROJECT_STATISTICS_H
