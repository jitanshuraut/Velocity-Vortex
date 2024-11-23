#include "Scheduler/scheduler.hpp"
#include "Executor-Manager-Core/OrderDetails.hpp"
#include <sstream>
#include <cstdlib>

OrderDetails parseOrder(const std::string &order)
{
    std::istringstream iss(order);
    OrderDetails details;
    iss >> details.side >> details.type >> details.timeInForce >> details.symbol >> details.qty;
    std::string limitPrice, stopPrice, trailPrice, trailPercent;
    if (iss >> limitPrice)
        details.limitPrice = limitPrice;
    if (iss >> stopPrice)
        details.stopPrice = stopPrice;
    if (iss >> trailPrice)
        details.trailPrice = trailPrice;
    if (iss >> trailPercent)
        details.trailPercent = trailPercent;
    return details;
}

void Scheduler::RoundRobin(std::string order)
{
    int total_provider = 3;
    int current = 0;
    while (true)
    {
        while (!orderQueue.empty())
        {
            int order_we_have = orderQueue.size();
            while (order_we_have--)
            {
                std::string order = orderQueue.pop();
                OrderDetails orderDetails = parseOrder(order);
                Provider_pointer[current]->push(orderDetails);
                current = (current + 1) % total_provider;
            }
        }
    }
}

void Scheduler::Equalizer(std::string order)
{
    int total_provider = 3;

    while (true)
    {
        while (!orderQueue.empty())
        {
            int order_we_have = orderQueue.size();
            while (order_we_have--)
            {
                std::string order = orderQueue.pop();
                OrderDetails orderDetails = parseOrder(order);
                int qty = std::atoi(orderDetails.qty.c_str());
                if (qty >= 3)
                {
                    for (int i = 0; i < total_provider - 1; i++)
                    {
                        OrderDetails individual_order = orderDetails;
                        individual_order.qty = std::to_string(qty / total_provider);
                        Provider_pointer[i]->push(individual_order);
                    }
                    OrderDetails last_loder = orderDetails;
                    last_loder.qty = std::to_string(qty / total_provider + qty % total_provider);
                    Provider_pointer[total_provider]->push(last_loder);
                }
                else
                {
                    Provider_pointer[0]->push(orderDetails);
                }
            }
        }
    }
}

void Scheduler::Latency_Based(std::string order)
{
    int total_provider = 3;
    while (true)
    {
        while (!orderQueue.empty())
        {
            int order_we_have = orderQueue.size();
            while (order_we_have--)
            {
                std::string order = orderQueue.pop();
                OrderDetails orderDetails = parseOrder(order);
                // considering provider 1 is fastest
                // consider provider 2 is 2nd fastest
                // consider provider 3 is slowest

                // totaly based on the tradeoff

                if (Provider_pointer[0]->size() <= 7)
                {
                    Provider_pointer[0]->push(orderDetails);
                }
                else if (Provider_pointer[2]->size() - Provider_pointer[1]->size() >= 10)
                {
                    Provider_pointer[1]->push(orderDetails);
                }
                else
                {
                    Provider_pointer[2]->push(orderDetails);
                }
            }
        }
    }
}

void Scheduler::Liquidity_Based(std::string order)
{
    int total_provider = 3;
    while (true)
    {
        while (!orderQueue.empty())
        {
            int order_we_have = orderQueue.size();
            while (order_we_have--)
            {
                std::string order = orderQueue.pop();
                OrderDetails orderDetails = parseOrder(order);

                //
            }
        }
    }
}
