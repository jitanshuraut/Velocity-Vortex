#ifndef QUOTE_HPP
#define QUOTE_HPP

#include <string>
#include <json/json.h> 
#include <iostream>
#include <vector>

class Quote {
public:
    std::string symbol;      
    double bidPrice;
    long bidSize;
    std::string bidExchange; 
    double askPrice;
    long askSize;            
    std::string askExchange; 
    std::string timestamp;   
    std::vector<std::string> conditions; 

    Quote(std::string sys):symbol{sys}{}

    void fromJson(const Json::Value& json) { 
        bidPrice = json["bp"].asDouble();
        bidSize = json["bs"].asInt64();
        bidExchange = json["bx"].asString(); 
        askPrice = json["ap"].asDouble();
        askSize = json["as"].asInt64();
        askExchange = json["ax"].asString(); 
        timestamp = json["t"].asString();

        for (const auto& condition : json["c"]) {
            conditions.push_back(condition.asString());
        }
    }

    void print() const {
        std::cout << "Symbol: " << symbol
                  << ", Bid Price: " << bidPrice
                  << ", Bid Size: " << bidSize
                  << ", Bid Exchange: " << bidExchange
                  << ", Ask Price: " << askPrice
                  << ", Ask Size: " << askSize
                  << ", Ask Exchange: " << askExchange
                  << ", Timestamp: " << timestamp
                  << ", Conditions: ";
        
        for (const auto& condition : conditions) {
            std::cout << condition << " ";
        }
        std::cout << std::endl;
    }
};

#endif 