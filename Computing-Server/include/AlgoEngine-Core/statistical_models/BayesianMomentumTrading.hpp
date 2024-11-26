#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>
#include "Utilities/SignalResult.hpp"

class BayesianMomentumTrading : public Indicator
{
public:
    BayesianMomentumTrading(const std::string &indicatorName, int period)
        : Indicator(indicatorName, period) {}

    SignalResult calculateSignal() override;
    static std::vector<double> calculateMomentum(const std::vector<OHLCV> &data, int period);
};
