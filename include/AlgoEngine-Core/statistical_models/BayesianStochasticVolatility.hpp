#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>
#include <cmath>
#include <numeric>
#include "Utilities/SignalResult.hpp"

class BayesianStochasticVolatility : public Indicator
{
public:
    BayesianStochasticVolatility(const std::string &indicatorName, int period)
        : Indicator(indicatorName, period) {}

    SignalResult calculateSignal() override;
    static std::vector<double> estimateVolatility(const std::vector<OHLCV> &data, int period);
};
