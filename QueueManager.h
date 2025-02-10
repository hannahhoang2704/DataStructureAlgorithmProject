//
// Created by Hanh Hoang on 8.2.2025.
//

#ifndef DATASTRUCTUREALGORITHMSPROJECT_QUEUEMANAGER_H
#define DATASTRUCTUREALGORITHMSPROJECT_QUEUEMANAGER_H
#include <deque>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include "InfoNode.h"

using namespace std;
class QueueManager{
public:
    QueueManager();
    void push_back(InfoNode& node);
    bool pop_data(InfoNode &node);
private:
    deque<InfoNode> node_queue;
    mutex queue_mutex;
    condition_variable data_available;

};
#endif //DATASTRUCTUREALGORITHMSPROJECT_QUEUEMANAGER_H
