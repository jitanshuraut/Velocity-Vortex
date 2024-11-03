#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>
#include "Utilities/SignalResult.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <cmath>

class BayesianKernelRegression : public Indicator
{
public:
    BayesianKernelRegression(const std::string &indicatorName, int period)
        : Indicator(indicatorName, period) {}

    SignalResult calculateSignal() override;
    static std::vector<double> performRegression(const std::vector<OHLCV> &data, int period);
    static std::chrono::system_clock::time_point parseTimestamp(const std::string &timestamp);
};
