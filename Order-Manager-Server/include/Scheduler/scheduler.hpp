#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP
#include <map>
#include <vector>
#include <string>
#include "Queue/TSQueue.hpp"
#include "Executor-Manager-Core/OrderDetails.hpp"
#include "Executor-Manager-Core/OrderDetails.hpp"

class Scheduler
{
public:
    void RoundRobin(std::string order);
    void Equalizer(std::string order);
    void Latency_Based(std::string order);
    void Liquidity_Based(std::string order);

    void Register_Provider(std::string name, TSQueue<OrderDetails> *Pointer_Queue)
    {
        Provider_Map[name] = Pointer_Queue;
        Provider_pointer.push_back(Pointer_Queue);
    }

private:
    std::map<std::string, TSQueue<OrderDetails> *> Provider_Map;
    std::vector<TSQueue<OrderDetails> *> Provider_pointer;
    TSQueue<std::string> orderQueue;
};

#endif
