#ifndef BAR_HPP
#define BAR_HPP

#include <json/json.h> 
#include <iostream>
#include <chrono>
#include "./OHLCV.hpp"

class Bar {
public:
    std::string time;
    std::string symbol; 
    double open;
    double high;
    double low;
    double close;
    long volume;

    // Constructor that initializes all member variables
    Bar(const std::string& sys)
        : symbol(sys), open(0.0), high(0.0), low(0.0), close(0.0), volume(0) {}

    void fromJson(const Json::Value& json) {
        time = json["t"].asString();
        open = json["o"].asDouble();
        high = json["h"].asDouble();
        low = json["l"].asDouble();
        close = json["c"].asDouble();
        volume = json["v"].asInt64();
    }

    void print() const {
        std::cout << "Time: " << time
                  << ", Symbol: " << symbol
                  << ", Open: " << open
                  << ", High: " << high
                  << ", Low: " << low
                  << ", Close: " << close
                  << ", Volume: " << volume << std::endl;
    }
};

#endif 
