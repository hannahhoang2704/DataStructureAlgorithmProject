//
// Created by Hanh Hoang on 8.2.2025.
//
#include <string>
#include <utility>


#ifndef DATASTRUCTUREALGORITHMSPROJECT_INFONODE_H
#define DATASTRUCTUREALGORITHMSPROJECT_INFONODE_H

using namespace std;
class InfoNode{
public:
    string name;
    uint64_t timestamps;
    float temp;
    InfoNode(string sensor_name="", uint64_t time=0, float value=0): name(std::move(sensor_name)), timestamps(time), temp(value){};
    InfoNode& operator=(const InfoNode& node){
        name = node.name;
        timestamps = node.timestamps;
        temp = node.temp;
        return *this;
    }

};


struct SensorInfo {
    std::string name;
    std::string fileName;
    int interval;

    SensorInfo(const string& n, const string& f, int i)
            : name(n), fileName(f), interval(i) {}
};

#endif //DATASTRUCTUREALGORITHMSPROJECT_INFONODE_H
