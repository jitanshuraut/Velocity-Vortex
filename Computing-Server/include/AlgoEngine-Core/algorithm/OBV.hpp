#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>

class OBV : public Indicator
{
public:
    OBV(const std::string &indicatorName)
        : Indicator(indicatorName, 1) {} 

    SignalResult calculateSignal() override; 
    static std::vector<double> calculate(const std::vector<OHLCV> &data);
};