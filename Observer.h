//
// Created by Hanh Hoang on 20.2.2025.
//

#ifndef DATASTRUCTUREALGORITHMSPROJECT_OBSERVER_H
#define DATASTRUCTUREALGORITHMSPROJECT_OBSERVER_H
#include "InfoNode.h"

class Observer{
public:
    virtual ~Observer()=default;
    virtual void update(InfoNode& node)=0;
};


class UIObserver:public Observer{
public:
    UIObserver(float& t1, float&t2, float &t3): temp1(t1), temp2(t2), temp3(t3){}
    void update(InfoNode& node) override{
        if(node.name == "sensor1"){
            temp1 = node.temp;
        }else if (node.name == "sensor2"){
            temp2 = node.temp;
        } else if (node.name == "sensor3"){
            temp3 = node.temp;
        }
    }
private:
    float &temp1;
    float &temp2;
    float & temp3;
};
#endif //DATASTRUCTUREALGORITHMSPROJECT_OBSERVER_H
