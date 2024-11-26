#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>

class MFI : public Indicator
{
public:
    MFI(const std::string &indicatorName, int period)
        : Indicator(indicatorName, period) {} 

    SignalResult calculateSignal() override;
    static std::vector<double> calculate(const std::vector<OHLCV> &data, int period);
};