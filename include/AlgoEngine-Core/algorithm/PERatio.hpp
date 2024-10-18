#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>

class PERatio : public Indicator
{
public:
    PERatio(const std::string &indicatorName, double earningsPerShare)
        : Indicator(indicatorName, 0), eps(earningsPerShare) {}

    SignalResult calculateSignal() override; 
    static std::vector<double> calculate(const std::vector<OHLCV> &data, double earningsPerShare);

private:
    double eps; 
};