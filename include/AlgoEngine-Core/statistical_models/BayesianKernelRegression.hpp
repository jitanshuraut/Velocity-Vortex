#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>
#include "Utilities/SignalResult.hpp"

class BayesianKernelRegression : public Indicator
{
public:
    BayesianKernelRegression(const std::string &indicatorName, int period)
        : Indicator(indicatorName, period) {}

    SignalResult calculateSignal() override;
    static std::vector<double> performRegression(const std::vector<OHLCV> &data, int period);
};
