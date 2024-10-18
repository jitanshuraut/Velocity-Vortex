#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>
#include <utility>

class StochasticOscillator : public Indicator
{
public:
    StochasticOscillator(const std::string &indicatorName, int period, int smoothK, int smoothD)
        : Indicator(indicatorName, period), smoothK(smoothK), smoothD(smoothD) {}

    SignalResult calculateSignal() override; 
    static std::vector<std::pair<double, double>> calculate(const std::vector<OHLCV> &historicalData, int period, int smoothK, int smoothD);

private:
    int smoothK;
    int smoothD;
};