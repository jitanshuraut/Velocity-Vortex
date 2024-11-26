#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>

class RSI : public Indicator {
public:
    RSI(const std::string& indicatorName, int period)
        : Indicator(indicatorName, period) {}

    SignalResult calculateSignal() override; 
    static std::vector<double> calculate(const std::vector<OHLCV>& historicalData, std::size_t period);
};