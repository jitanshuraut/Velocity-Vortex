#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>
#include <map>
#include "Utilities/SignalResult.hpp"

class BayesianCausalInferenceTrading : public Indicator
{
public:
    BayesianCausalInferenceTrading(const std::string &indicatorName, int period)
        : Indicator(indicatorName, period) {}

    SignalResult calculateSignal() override;
    static std::map<std::string, double> performCausalInference(const std::vector<OHLCV> &data, int period);
};
