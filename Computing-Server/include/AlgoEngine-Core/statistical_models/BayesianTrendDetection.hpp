#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>
#include "Utilities/SignalResult.hpp"
#include <cmath>
#include <numeric>

class BayesianTrendDetection : public Indicator
{
public:
    BayesianTrendDetection(const std::string &indicatorName, int period)
        : Indicator(indicatorName, period) {}

    SignalResult calculateSignal() override;
    static std::vector<double> calculate(const std::vector<OHLCV> &data, int period);
};
