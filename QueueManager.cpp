//
// Created by Hanh Hoang on 8.2.2025.
//
#include "QueueManager.h"

QueueManager::QueueManager() {}

void QueueManager::push_back(InfoNode& node) {
    lock_guard<mutex> lock(queue_mutex);
    node_queue.push_back(node);

    //notify observers once new node is pushed to queue
    for(auto o: observers){
        o->update(node);
    }
}

bool QueueManager::pop_data(InfoNode &node) {
    unique_lock<mutex> lock(queue_mutex);
    if (!node_queue.empty()){
        node = node_queue.front();
        node_queue.pop_front();
        return true;
    }else{
        return false;
    }
}

void QueueManager::add_observer(Observer *observer) {
    observers.push_back(observer);
}