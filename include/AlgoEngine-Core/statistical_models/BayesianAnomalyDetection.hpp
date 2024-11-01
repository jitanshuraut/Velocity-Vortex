#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>
#include "Utilities/SignalResult.hpp"

class BayesianAnomalyDetection : public Indicator
{
public:
    BayesianAnomalyDetection(const std::string &indicatorName, int period)
        : Indicator(indicatorName, period) {}

    SignalResult calculateSignal() override;
    static std::vector<double> detectAnomalies(const std::vector<OHLCV> &data, int period);
};
