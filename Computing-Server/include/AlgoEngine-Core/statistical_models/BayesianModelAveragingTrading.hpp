#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>
#include <map>
#include "Utilities/SignalResult.hpp"

class BayesianModelAveragingTrading : public Indicator
{
public:
    BayesianModelAveragingTrading(const std::string &indicatorName, int period)
        : Indicator(indicatorName, period) {}

    SignalResult calculateSignal() override;
    static double computeModelAverage(const std::vector<double> &modelPredictions, const std::vector<double> &weights);
};
