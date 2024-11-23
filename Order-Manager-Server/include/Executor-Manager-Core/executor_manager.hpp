#ifndef ORDERMANAGER_HPP
#define ORDERMANAGER_HPP

#include "order.hpp"
#include "APIClient.hpp"
#include <json/json.h>
#include <string>
#include <iostream>
#include <optional>
#include <map>
#include "Queue/TSQueue.hpp"
#include "OrderDetails.hpp"

class ExecutorManager
{
private:
    APIClient apiClient;
    TSQueue<OrderDetails> orderQueue;

public:
    ExecutorManager()
        : apiClient(std::getenv("APCA_API_KEY_ID"), std::getenv("APCA_API_SECRET_KEY")) {}

    void createOrder(OrderDetails order_Details);

    void processOrderQueue();
    void getAllOrders();
    void deleteAllOrders();
    void getOrderById(const std::string &orderId);
    void deleteOrderById(const std::string &orderId);

    std::string getOpenPosition(const std::string &symbol);
    std::string closePosition(const std::string &symbol);

    TSQueue<OrderDetails> *getOrderQueuePointer()
    {
        return &orderQueue;
    }

private:
    Order parseOrderFromJson(const std::string &response);
};

#endif