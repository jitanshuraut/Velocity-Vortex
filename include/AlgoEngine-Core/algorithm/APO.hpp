#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>

class APO : public Indicator
{
public:
    APO(const std::string &indicatorName, int shortPeriod, int longPeriod)
        : Indicator(indicatorName, shortPeriod), longPeriod(longPeriod) {}

    SignalResult calculateSignal() override; 
    static std::vector<double> calculate(const std::vector<OHLCV> &ohlcvData, int shortPeriod, int longPeriod);

private:
    int longPeriod;
};