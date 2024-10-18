#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>

class CCI : public Indicator
{
public:
    CCI(const std::string &indicatorName, int period)
        : Indicator(indicatorName, period) {}

    SignalResult calculateSignal() override;
     std::vector<double> calculate(const std::vector<OHLCV> &data, int period);

private:
    void calculateCCIRange(size_t start, size_t end, const std::vector<OHLCV> &data, double constant, std::vector<double> &cciValues);
};