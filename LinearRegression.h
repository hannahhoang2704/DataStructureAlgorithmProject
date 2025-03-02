//
// Created by Hanh Hoang on 24.2.2025.
//

#ifndef DATASTRUCTUREALGORITHMSPROJECT_LINEARREGRESSION_H
#define DATASTRUCTUREALGORITHMSPROJECT_LINEARREGRESSION_H

#include <iostream>
#include <vector>
#include <map>
#include <mutex>

using namespace std;
class LinearRegression {
private:
    vector<float> time;  // Stores timestamps (converted to seconds)
    vector<float> values; // Stores corresponding sensor values

    float coeff;       // Slope (rate of change)
    float constTerm;   // Intercept (starting value)
    float sum_xy, sum_x, sum_y, sum_x_square;
    mutex reg_mutex;   // Protects regression updates

public:
    LinearRegression() : coeff(0), constTerm(0), sum_xy(0), sum_x(0), sum_y(0), sum_x_square(0) {}

    // Add new data points for regression
    void addData(uint64_t timestamp, float value, uint64_t start_time);
    void clearData();
    bool trainModel();
    float predict_future(uint64_t future_timestamp, uint64_t start_time);
};


#endif //DATASTRUCTUREALGORITHMSPROJECT_LINEARREGRESSION_H
