//
// Created by Hanh Hoang on 24.2.2025.
//

#include "LinearRegression.h"
#define REGRESSION_DATA_SIZE 5
void LinearRegression::addData(uint64_t timestamp, float value, uint64_t start_time) {
    lock_guard<mutex> lock(reg_mutex);
    float t = (timestamp - start_time) / 1000.0f; // Convert to seconds
    time.push_back(t);
    values.push_back(value);
    sum_xy += t * value;
    sum_x += t;
    sum_y += value;
    sum_x_square += t * t;
}

void LinearRegression::clearData() {
    time.clear();
    values.clear();
    sum_x = 0;
    sum_xy = 0;
    sum_y = 0;
    sum_x_square = 0;
    coeff = 0;
    constTerm = 0;
}

// Train (calculate coefficients and constant term if there's enough data)
bool LinearRegression::trainModel() {
    lock_guard<mutex> lock(reg_mutex);
    size_t dataSize = time.size();
    if (dataSize < REGRESSION_DATA_SIZE) {
        cerr << "Not enough data to train the model!" << endl;
        return false;
    }

    float N = REGRESSION_DATA_SIZE;
    float denominator = (N * sum_x_square - sum_x * sum_x);
    if (denominator == 0) {
        cerr << "Regression denominator is zero!" << endl;
        return false;
    }
    // Calculate coefficient and constant term
    coeff = (N * sum_xy - sum_x * sum_y) / denominator;
    constTerm = (sum_y * sum_x_square - sum_x * sum_xy) / denominator;
    return true;
}

// Predict future sensor value at given timestamp
float LinearRegression::predict_future(uint64_t future_timestamp, uint64_t start_time) {
    lock_guard<mutex> lock(reg_mutex);
    float t = (float)(future_timestamp - start_time) / 1000.0f;
    return coeff * t + constTerm;
}
