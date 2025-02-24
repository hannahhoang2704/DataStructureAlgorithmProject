//
// Created by Hanh Hoang on 24.2.2025.
//

#include "LinearRegression.h"
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

// Train (calculate coefficients)
void LinearRegression::trainModel() {
    lock_guard<mutex> lock(reg_mutex);
    if (time.size() < 2) {
        cerr << "Not enough data to train the model!" << endl;
        return;
    }

    float N = time.size();
    float denominator = (N * sum_x_square - sum_x * sum_x);
    if (denominator == 0) {
        cerr << "Regression denominator is zero!" << endl;
        return;
    }

    coeff = (N * sum_xy - sum_x * sum_y) / denominator;
    constTerm = (sum_y * sum_x_square - sum_x * sum_xy) / denominator;
}

// Predict future sensor value at given timestamp
float LinearRegression::predict_future(uint64_t future_timestamp, uint64_t start_time) {
    lock_guard<mutex> lock(reg_mutex);
    float t = (future_timestamp - start_time) / 1000.0f;
    return coeff * t + constTerm;
}