#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>

class ATR : public Indicator
{
public:
    ATR(const std::string &indicatorName, int period)
        : Indicator(indicatorName, period) {}

    SignalResult calculateSignal() override;
    std::vector<double> calculate(const std::vector<OHLCV> &ohlcvData, int period);

private:
    double previousClose = 0.0;
};