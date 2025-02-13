//
// Created by Hanh Hoang on 8.2.2025.
//
#include <string>


#ifndef DATASTRUCTUREALGORITHMSPROJECT_INFONODE_H
#define DATASTRUCTUREALGORITHMSPROJECT_INFONODE_H

using namespace std;
class InfoNode{
private:
    std::string name;
    uint64_t timestamp;
    float temperature;
public:
    InfoNode(std::string name, uint64_t timestamp, float temperature) : name(name), timestamp(timestamp), temperature(temperature) {}

    std::string get_name() const { return name; }
    uint64_t get_timestamp() const { return timestamp; }
    float get_temperature() const { return temperature; }
};
#endif //DATASTRUCTUREALGORITHMSPROJECT_INFONODE_H
