#include "Executor-Manager-Core/executor_manager.hpp"
#include <optional>
#include <json/json.h>

void ExecutorManager::createOrder(OrderDetails order_Details)
{
    std::string url = "https://paper-api.alpaca.markets/v2/orders";

    Json::Value jsonPayload;
    jsonPayload["side"] = order_Details.side;
    jsonPayload["type"] = order_Details.type;
    jsonPayload["time_in_force"] = order_Details.timeInForce;
    jsonPayload["symbol"] = order_Details.symbol;
    jsonPayload["qty"] = order_Details.qty;

    if (order_Details.limitPrice)
        jsonPayload["limit_price"] = *order_Details.limitPrice;
    if (order_Details.stopPrice)
        jsonPayload["stop_price"] = *order_Details.stopPrice;
    if (order_Details.trailPrice)
        jsonPayload["trail_price"] = *order_Details.trailPrice;
    if (order_Details.trailPercent)
        jsonPayload["trail_percent"] = *order_Details.trailPercent;

    Json::StreamWriterBuilder writer;
    std::string jsonString = Json::writeString(writer, jsonPayload);

    auto [ret, response] = apiClient.post(url, jsonString);
    if (ret == CURLE_OK)
    {
        // try
        // {
        //     Order order = parseOrderFromJson(response);
        //     GlobalQueue::getInstance().enqueue(order);
        // }
        // catch (const std::exception &e)
        // {
        //     std::cerr << "Error processing response: " << e.what() << std::endl;
        // }
    }
    else
    {
        std::cerr << "Error creating order: " << curl_easy_strerror(ret) << std::endl;
    }
}

void ExecutorManager::processOrderQueue()
{
    while (true)
    {
        while (!orderQueue.empty())
        {
            int n = orderQueue.size();
            while (n--)
            {
                OrderDetails order = orderQueue.pop();
                createOrder(order);
            }
        }
    }
}

void ExecutorManager::getAllOrders()
{
    std::string url = "https://paper-api.alpaca.markets/v2/orders?status=open";
    auto [ret, response] = apiClient.get(url);

    if (ret == CURLE_OK)
    {
        Json::CharReaderBuilder reader;
        Json::Value jsonResponse;
        std::istringstream s(response);
        std::string errs;

        if (Json::parseFromStream(reader, s, &jsonResponse, &errs))
        {
            for (const auto &item : jsonResponse)
            {
                Order order = parseOrderFromJson(item.toStyledString());
                order.display();
            }
        }
        else
        {
            std::cerr << "Error parsing JSON response: " << errs << std::endl;
        }
    }
    else
    {
        std::cerr << "Error getting all orders: " << curl_easy_strerror(ret) << std::endl;
    }
}

void ExecutorManager::deleteAllOrders()
{
    std::string url = "https://paper-api.alpaca.markets/v2/orders";
    auto [ret, response] = apiClient.del(url);
    if (ret != CURLE_OK)
    {
        std::cerr << "Error deleting all orders: " << curl_easy_strerror(ret) << std::endl;
    }
}

void ExecutorManager::getOrderById(const std::string &orderId)
{
    std::string url = "https://paper-api.alpaca.markets/v2/orders/" + orderId;
    auto [ret, response] = apiClient.get(url);

    if (ret == CURLE_OK)
    {
        try
        {

            Order order = parseOrderFromJson(response);
            order.display();
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error processing response: " << e.what() << std::endl;
        }
    }
    else
    {
        std::cerr << "Error getting order by ID: " << curl_easy_strerror(ret) << std::endl;
    }
}

void ExecutorManager::deleteOrderById(const std::string &orderId)
{
    std::string url = "https://paper-api.alpaca.markets/v2/orders/" + orderId;
    auto [ret, response] = apiClient.del(url);
    if (ret != CURLE_OK)
    {
        std::cerr << "Error deleting order by ID: " << curl_easy_strerror(ret) << std::endl;
    }
}

std::string ExecutorManager::getOpenPosition(const std::string &symbol)
{
    std::string url = "https://api.alpaca.markets/v2/positions/" + symbol;
    auto [curlCode, jsonResponse] = apiClient.get(url);

    if (curlCode != CURLE_OK)
    {
        return "Failed to fetch position for " + symbol + ". CURLcode: " + std::to_string(curlCode);
    }
    return jsonResponse;
}

std::string ExecutorManager::closePosition(const std::string &symbol)
{
    std::string url = "https://api.alpaca.markets/v2/positions/" + symbol;
    auto [curlCode, jsonResponse] = apiClient.del(url);

    if (curlCode != CURLE_OK)
    {
        return "Failed to close position for " + symbol + ". CURLcode: " + std::to_string(curlCode);
    }

    return jsonResponse;
}

Order ExecutorManager::parseOrderFromJson(const std::string &response)
{
    Json::CharReaderBuilder reader;
    Json::Value jsonResponse;
    std::istringstream s(response);
    std::string errs;

    if (!Json::parseFromStream(reader, s, &jsonResponse, &errs))
    {
        throw std::runtime_error("Failed to parse JSON response: " + errs);
    }

    std::string orderId = jsonResponse["id"].asString();
    std::string clientOrderId = jsonResponse["client_order_id"].asString();
    std::string createdAt = jsonResponse["created_at"].asString();
    std::string updatedAt = jsonResponse["updated_at"].asString();
    std::string submittedAt = jsonResponse["submitted_at"].asString();
    std::string filledAt = jsonResponse["filled_at"].asString();
    std::string expiredAt = jsonResponse["expired_at"].asString();
    std::string canceledAt = jsonResponse["canceled_at"].asString();
    std::string failedAt = jsonResponse["failed_at"].asString();
    std::string replacedAt = jsonResponse["replaced_at"].asString();
    std::string assetId = jsonResponse["asset_id"].asString();
    std::string symbol = jsonResponse["symbol"].asString();
    std::string assetClass = jsonResponse["asset_class"].asString();
    std::string qty = jsonResponse["qty"].asString();
    std::string filledQty = jsonResponse["filled_qty"].asString();
    std::string filledAvgPrice = jsonResponse["filled_avg_price"].asString();
    std::string orderClass = jsonResponse["order_class"].asString();
    std::string orderType = jsonResponse["order_type"].asString();
    std::string side = jsonResponse["side"].asString();
    std::string positionIntent = jsonResponse["position_intent"].asString();
    std::string timeInForce = jsonResponse["time_in_force"].asString();
    std::string limitPrice = jsonResponse["limit_price"].asString();
    std::string stopPrice = jsonResponse["stop_price"].asString();
    std::string trailPrice = jsonResponse["trail_price"].asString();
    std::string trailPercent = jsonResponse["trail_percent"].asString();
    bool extendedHours = jsonResponse["extended_hours"].asBool();

    return Order(orderId, clientOrderId, createdAt, updatedAt, submittedAt, filledAt,
                 expiredAt, canceledAt, failedAt, replacedAt, assetId, symbol,
                 assetClass, qty, filledQty, filledAvgPrice, orderClass,
                 orderType, side, positionIntent, timeInForce,
                 limitPrice, stopPrice, trailPrice, trailPercent, extendedHours);
}
