#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>
#include <cmath>
#include <stdexcept>
#include "Utilities/SignalResult.hpp"

class BayesianGARCHTrading : public Indicator
{
public:
    BayesianGARCHTrading(const std::string &indicatorName, int period)
        : Indicator(indicatorName, period) {}

    SignalResult calculateSignal() override;
    static std::vector<double> estimateGARCH(const std::vector<OHLCV> &data, int period);
};
