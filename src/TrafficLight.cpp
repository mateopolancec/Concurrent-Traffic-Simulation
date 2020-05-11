#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <ctime>
#include <future>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> lock(_mu);
    _condition.wait(lock, [this] {
        return !_queue.empty();
    });

    T msg = std::move(_queue.front());
    _queue.pop_front();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    
    std::unique_lock<std::mutex> lock(_mu);
    _queue.emplace_back(msg);
    _condition.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    
    while (true)
    {
        // adding this line to avoid a lot of calls unnecessary:
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto msg = _queue.receive();
        if (msg == TrafficLightPhase::green)
        {
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    
    std::random_device rd;

    // Engines
    //
    std::mt19937 e2(rd());
    //std::knuth_b e2(rd());
    //std::default_random_engine e2(rd()) ;

    //
    // Distribtuions
    //
    std::uniform_int_distribution<int> dist(4000, 6000);
    // std::normal_distribution<> dist(4000, 6000);
    //std::student_t_distribution<> dist(5);
    //std::poisson_distribution<> dist(2);
    //std::extreme_value_distribution<> dist(0,2);

    int randomCycleValue = dist(e2);

    std::chrono::time_point<std::chrono::system_clock> timeNow;

    timeNow = std::chrono::system_clock::now();
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        int elapsed_seconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - timeNow).count();

        if (elapsed_seconds >= randomCycleValue)
        {
            if (this->_currentPhase == TrafficLightPhase::red)
            {
                this->_currentPhase = TrafficLightPhase::green;
            }
            else
            {
                this->_currentPhase = TrafficLightPhase::red;
            }

            auto future = std::async(std::launch::async, &MessageQueue<TrafficLightPhase>::send, &_queue, std::move(_currentPhase));
            future.wait();

            randomCycleValue = dist(e2);
            timeNow = std::chrono::system_clock::now();
        }
    }
}