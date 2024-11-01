#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>
#include "Utilities/SignalResult.hpp"

class BayesianDecisionTreeTrading : public Indicator
{
public:
    BayesianDecisionTreeTrading(const std::string &indicatorName, int period)
        : Indicator(indicatorName, period) {}

    SignalResult calculateSignal() override;
    static std::vector<double> evaluateDecisionTree(const std::vector<OHLCV> &data, int period);
};
