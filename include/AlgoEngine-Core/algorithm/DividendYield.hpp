#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>

class DividendYield : public Indicator
{
public:
    DividendYield(const std::string &indicatorName, double dividendPerShare)
        : Indicator(indicatorName, 0), dps(dividendPerShare) {}

    SignalResult calculateSignal() override; 
    static std::vector<double> calculate(const std::vector<OHLCV> &data, double dividendPerShare);

private:
    double dps; 
};