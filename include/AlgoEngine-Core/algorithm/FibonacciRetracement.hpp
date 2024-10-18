#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>

class FibonacciRetracement : public Indicator
{
public:
    FibonacciRetracement(const std::string &indicatorName)
        : Indicator(indicatorName, 0) {}

    SignalResult calculateSignal() override; 
    static std::vector<double> calculate(const std::vector<OHLCV> &data);
};