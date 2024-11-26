#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>
#include "Utilities/SignalResult.hpp"

class BayesianCorrelationEstimation : public Indicator
{
public:
    BayesianCorrelationEstimation(const std::string &indicatorName, int period)
        : Indicator(indicatorName, period) {}

    SignalResult calculateSignal() override;
    static std::vector<double> calculate(const std::vector<OHLCV> &data1, const std::vector<OHLCV> &data2, int period);
};
