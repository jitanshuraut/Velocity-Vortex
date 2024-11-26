#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>
#include <stdexcept>
#include "Utilities/SignalResult.hpp"

class SARIMA : public Indicator
{
private:
    int p;    // Order of the autoregressive part
    int d;    // Degree of differencing
    int q;    // Order of the moving average part
    int P;    // Seasonal order of the autoregressive part
    int D;    // Seasonal degree of differencing
    int Q;    // Seasonal order of the moving average part
    int s;    // Seasonal period

public:
    SARIMA(const std::string &indicatorName, int p, int d, int q, int P, int D, int Q, int s)
        : Indicator(indicatorName, p + d + q + P + D + Q), p(p), d(d), q(q), P(P), D(D), Q(Q), s(s) {}

    SignalResult calculateSignal() override;
    static std::vector<double> fitSARIMA(const std::vector<OHLCV> &data, int p, int d, int q, int P, int D, int Q, int s);
};
