#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"


class Vehicle;


template <class T>
class MessageQueue
{
public:
    T receive();
    void send(T&& msg);
private:
    std::deque<T> _queue;
    std::condition_variable _condition;
    std::mutex _mu;
};


enum class TrafficLightPhase
{
    red,
    green
};

class TrafficLight: public TrafficObject
{
public:
    TrafficLight();
    TrafficLightPhase getCurrentPhase();
    void waitForGreen();
    void simulate();

private:
    void cycleThroughPhases();
    std::condition_variable _condition;
    std::mutex _mutex;
    TrafficLightPhase _currentPhase;
    MessageQueue<TrafficLightPhase> _queue;
};

#endif