#ifndef TRADE_HPP
#define TRADE_HPP

#include <string>
#include <json/json.h>
#include <iostream>
#include <vector>

class Trade
{
public:
    std::string symbol;
    double price;
    long size;
    std::string timestamp;
    std::vector<std::string> conditions;
    int identifier;
    std::string exchange;
    std::string tradeCondition;

    explicit Trade(std::string sys) : symbol{sys} {}

    // Parses JSON data to populate the Trade object.
    // @param json: JSON object containing trade data.
    void fromJson(const Json::Value &json)
    {
        price = json["p"].asDouble();
        size = json["s"].asInt64();
        timestamp = json["t"].asString();

        if (json.isMember("c") && json["c"].isArray())
        {
            for (const auto &condition : json["c"])
            {
                conditions.push_back(condition.asString());
            }
        }

        identifier = json["i"].asInt();
        exchange = json["x"].asString();
        tradeCondition = json["z"].asString();
    }

    // Prints the trade details to the standard output.
    void print() const
    {
        std::cout << "Symbol: " << symbol
                  << ", Price: " << price
                  << ", Size: " << size
                  << ", Timestamp: " << timestamp
                  << ", Identifier: " << identifier
                  << ", Exchange: " << exchange
                  << ", Trade Condition: " << tradeCondition
                  << ", Conditions: ";

        for (const auto &condition : conditions)
        {
            std::cout << condition << " ";
        }
        std::cout << std::endl;
    }
};

#endif // TRADE_HPP