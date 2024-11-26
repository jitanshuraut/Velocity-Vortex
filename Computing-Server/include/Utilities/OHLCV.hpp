#pragma once
#include <string>


struct OHLCV {
    double open;
    double high;
    double low;
    double close;
    double volume;
    std::string timestamp;
};