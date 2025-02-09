//
// Created by Hanh Hoang on 8.2.2025.
//
#include "QueueManager.h"

QueueManager::QueueManager() {}

void QueueManager::push_back(InfoNode& node) {
    lock_guard<mutex> lock(queue_mutex);
    node_queue.push_back(node);
    data_available.notify_one();
}

bool QueueManager::pop_data(InfoNode &node) {
    unique_lock<mutex> lock(queue_mutex);
    data_available.wait(lock, [this](){return !node_queue.empty();});
    node = node_queue.front();
    node_queue.pop_front();
    return true;
}