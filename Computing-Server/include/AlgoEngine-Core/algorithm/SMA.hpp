#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>

class SMA : public Indicator
{
public:
    SMA(const std::string &indicatorName, int period)
        : Indicator(indicatorName, period) {}

    SignalResult calculateSignal() override;
    static std::vector<double> calculate(const std::vector<OHLCV> &historicalData, int period);

private:
    static const size_t PARALLEL_THRESHOLD = 1000;
};