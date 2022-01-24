#include <iostream>
#include <random>
#include <chrono>
#include "TrafficLight.h"

using std::chrono::steady_clock;

/* Implementation of class "MessageQueue" */

/* 
template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
}
*/

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> lock_g(_mutex);
    _queue.emplace_back(std::move(msg));
    _condition.notify_one();
}


/* Implementation of class "TrafficLight" */

/* 
TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}
*/
void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    auto t = std::thread(&TrafficLight::cycleThroughPhases, this);
    threads.emplace_back(std::move(t));
}


// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
    steady_clock::time_point timer_start = steady_clock::now();
    steady_clock::time_point timer_end = steady_clock::now();
    
    typedef std::mt19937 RNG;
    uint32_t seed;  
    RNG rng;
    rng.seed(seed);
    std::uniform_int_distribution<uint32_t> rng_range(4000,6000);
    auto cycle_time = rng_range(rng);
    while (true) {
        auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(timer_end - timer_start).count();
        if (elapsed_time >= cycle_time) {
            _currentPhase = (_currentPhase == TrafficLightPhase::red) ? TrafficLightPhase::green : TrafficLightPhase::red;
            _msg_queue.send(std::move(_currentPhase));
            timer_start = steady_clock::now();
            cycle_time = rng_range(rng);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        timer_end = steady_clock::now();
    }
}

