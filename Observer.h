//
// Created by Hanh Hoang on 20.2.2025.
//

#ifndef DATASTRUCTUREALGORITHMSPROJECT_OBSERVER_H
#define DATASTRUCTUREALGORITHMSPROJECT_OBSERVER_H
#include <map>
#include <mutex>
#include "InfoNode.h"

using namespace std;
class Observer{
public:
    virtual ~Observer()=default;
    virtual void update(InfoNode& node)=0;
};


class UIObserver:public Observer{
public:
    UIObserver(map<string, float>&map, mutex& data_mutex):sensor_map(map), data_mutex(data_mutex){};
    void update(InfoNode& node) override{
        lock_guard<mutex> lock(data_mutex);
        sensor_map[node.name] = node.temp;
    }
private:
    map<string, float>&sensor_map;
    mutex& data_mutex;

};
#endif //DATASTRUCTUREALGORITHMSPROJECT_OBSERVER_H
