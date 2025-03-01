//
// Created by Hanh Hoang on 25.2.2025.
//

#ifndef DATASTRUCTUREALGORITHMSPROJECT_STATISTIC_H
#define DATASTRUCTUREALGORITHMSPROJECT_STATISTIC_H
#include <iostream>
#include "DatabaseStorage.h"
#include "LinearRegression.h"
#include "InfoNode.h"

using namespace std;
class Statistic {
private:
    LinearRegression& linear_regression;
    DatabaseStorage& db_reader;
public:
    Statistic(LinearRegression &linear_reg, DatabaseStorage &db_reader): linear_regression(linear_reg), db_reader(db_reader){};
    bool predict_future_temp(const string& sensor, uint64_t interval, float& predict_temp_val);
    void preparePlotData(const string& sensorName,
                         const map<string, vector<uint64_t>>& timestamps,
                         const map<string, vector<float>>& values,
                         vector<float>& timeInSeconds, vector<float>& sensorValues);
};


#endif //DATASTRUCTUREALGORITHMSPROJECT_STATISTIC_H
