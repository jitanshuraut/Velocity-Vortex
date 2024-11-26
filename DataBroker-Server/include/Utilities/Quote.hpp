#ifndef QUOTE_HPP
#define QUOTE_HPP

#include <string>
#include <json/json.h>
#include <iostream>
#include <vector>

class Quote
{
public:
    std::string symbol;
    double bid_price;
    long bid_size;
    std::string bid_exchange;
    double ask_price;
    long ask_size;
    std::string ask_exchange;
    std::string timestamp;
    std::vector<std::string> conditions;

    explicit Quote(std::string sys) : symbol{sys} {}

    // Parses JSON object to populate Quote fields.
    // Input: json - JSON object containing quote data.
    void FromJson(const Json::Value &json)
    {
        bid_price = json["bp"].asDouble();
        bid_size = json["bs"].asInt64();
        bid_exchange = json["bx"].asString();
        ask_price = json["ap"].asDouble();
        ask_size = json["as"].asInt64();
        ask_exchange = json["ax"].asString();
        timestamp = json["t"].asString();

        for (const auto &condition : json["c"])
        {
            conditions.push_back(condition.asString());
        }
    }

    // Prints the Quote details to the standard output.
    void Print() const
    {
        std::cout << "Symbol: " << symbol
                  << ", Bid Price: " << bid_price
                  << ", Bid Size: " << bid_size
                  << ", Bid Exchange: " << bid_exchange
                  << ", Ask Price: " << ask_price
                  << ", Ask Size: " << ask_size
                  << ", Ask Exchange: " << ask_exchange
                  << ", Timestamp: " << timestamp
                  << ", Conditions: ";

        for (const auto &condition : conditions)
        {
            std::cout << condition << " ";
        }
        std::cout << std::endl;
    }
};

#endif // QUOTE_HPP