#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>
#include <cmath>
#include <stdexcept>
#include "Utilities/SignalResult.hpp"

class ARIMA : public Indicator
{
private:
    int p; // Order of the autoregressive part
    int d; // Degree of differencing
    int q; // Order of the moving average part

public:
    ARIMA(const std::string &indicatorName, int p, int d, int q)
        : Indicator(indicatorName, p + d + q), p(p), d(d), q(q) {}

    SignalResult calculateSignal() override;
    static std::vector<double> fitARIMA(const std::vector<OHLCV> &data, int p, int d, int q);
};
