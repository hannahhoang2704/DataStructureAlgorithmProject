//
// Created by Hanh Hoang on 8.2.2025.
//

#ifndef DATASTRUCTUREALGORITHMSPROJECT_QUEUEMANAGER_H
#define DATASTRUCTUREALGORITHMSPROJECT_QUEUEMANAGER_H
#include <deque>
#include <iostream>
#include <mutex>
#include <vector>
#include <condition_variable>
#include "InfoNode.h"
#include "Observer.h"

using namespace std;
class QueueManager{
public:
    QueueManager();
    void push_back(InfoNode& node);
    bool pop_data(InfoNode &node);
    bool is_empty();
    void add_observer(Observer* observer);
private:
    deque<InfoNode> node_queue;
    mutex queue_mutex;
    condition_variable data_available;
    vector<Observer*> observers;
};
#endif //DATASTRUCTUREALGORITHMSPROJECT_QUEUEMANAGER_H
