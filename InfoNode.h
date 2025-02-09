//
// Created by Hanh Hoang on 8.2.2025.
//
#include <string>


#ifndef DATASTRUCTUREALGORITHMSPROJECT_INFONODE_H
#define DATASTRUCTUREALGORITHMSPROJECT_INFONODE_H

using namespace std;
class InfoNode{
public:
    string name;
    uint64_t timestamps;
    int temp;
    InfoNode(string sensor_name, uint64_t time, int value): name(sensor_name), timestamps(time), temp(value){};
    InfoNode& operator=(const InfoNode& node){
        name = node.name;
        timestamps = node.timestamps;
        temp = node.temp;
        return *this;
    }

};
#endif //DATASTRUCTUREALGORITHMSPROJECT_INFONODE_H
