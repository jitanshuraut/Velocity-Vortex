#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>

class EMA : public Indicator {
public:
    EMA(const std::string& indicatorName, int period)
        : Indicator(indicatorName, period) {}

    SignalResult calculateSignal() override; 
    static std::vector<double> calculate(const std::vector<OHLCV>& data, std::size_t period);
};