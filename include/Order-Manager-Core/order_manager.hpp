#ifndef ORDERMANAGER_HPP
#define ORDERMANAGER_HPP

#include "order.hpp"
#include "APIClient.hpp"
#include <json/json.h>
#include <string>
#include <iostream>
#include <optional>
#include <map>

class OrderManager
{
private:
    APIClient apiClient;

public:
    OrderManager()
        : apiClient(std::getenv("APCA_API_KEY_ID"), std::getenv("APCA_API_SECRET_KEY")) {}

    void createOrder(const std::string &side, const std::string &type, const std::string &timeInForce,
                     const std::string &symbol, const std::string &qty,
                     const std::optional<std::string> &limitPrice,
                     const std::optional<std::string> &stopPrice,
                     const std::optional<std::string> &trailPrice,
                     const std::optional<std::string> &trailPercent);

    void processOrderQueue();
    void getAllOrders();
    void deleteAllOrders();
    void getOrderById(const std::string &orderId);
    void deleteOrderById(const std::string &orderId);
    
    std::map<std::string, std::string> getMarketInfo();
    std::string getAllOpenPositions();
    std::string closeAllPositions();
    std::string getOpenPosition(const std::string &symbol);
    std::string closePosition(const std::string &symbol);

private:
    Order parseOrderFromJson(const std::string &response);
};

#endif