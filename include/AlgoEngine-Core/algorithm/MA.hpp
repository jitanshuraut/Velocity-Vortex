#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>

class MA : public Indicator
{
public:
    MA(const std::string &indicatorName, int period)
        : Indicator(indicatorName, period) {}

    SignalResult calculateSignal() override; 
    static std::vector<double> calculate(const std::vector<OHLCV> &data, int period);
};