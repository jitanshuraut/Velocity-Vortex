#include "AlgoEngine-Core/algorithm/StochasticOscillator.hpp"
#include <algorithm>
#include <numeric>
#include <stdexcept>

SignalResult StochasticOscillator::calculateSignal()
{
    if (historicalData.size() < period)
    {
        throw std::runtime_error("Not enough data to calculate Stochastic Oscillator.");
    }

    double currentClose = historicalData.back().close;
    double lowestLow = 1e9;
    double highestHigh = -1e9;

    for (size_t i = historicalData.size() - period; i < historicalData.size(); ++i)
    {
        lowestLow = std::min(lowestLow, historicalData[i].low);
        highestHigh = std::max(highestHigh, historicalData[i].high);
    }

    double k = (currentClose - lowestLow) / (highestHigh - lowestLow) * 100.0;
    signals.push_back(SignalResult(k));
    return SignalResult(signals.back());
}

std::vector<std::pair<double, double>> StochasticOscillator::calculate(const std::vector<OHLCV> &historicalData, int period, int smoothK, int smoothD)
{
    std::vector<std::pair<double, double>> stochValues;

    if (historicalData.size() < period + smoothK + smoothD)
    {
        throw std::runtime_error("Not enough data to calculate Stochastic Oscillator.");
    }

    std::vector<double> kValues;

    for (size_t i = period - 1; i < historicalData.size(); ++i)
    {
        double currentClose = historicalData[i].close;
        double lowestLow = std::numeric_limits<double>::max();
        double highestHigh = std::numeric_limits<double>::lowest();

        for (size_t j = i - period + 1; j <= i; ++j)
        {
            lowestLow = std::min(lowestLow, historicalData[j].low);
            highestHigh = std::max(highestHigh, historicalData[j].high);
        }

        double k = (currentClose - lowestLow) / (highestHigh - lowestLow) * 100.0;
        kValues.push_back(k);
    }

    for (size_t i = smoothK - 1; i < kValues.size(); ++i)
    {
        double kSum = std::accumulate(kValues.begin() + (i - smoothK + 1), kValues.begin() + (i + 1), 0.0);
        double smoothedK = kSum / smoothK;
        stochValues.emplace_back(smoothedK, 0.0);
    }

    for (size_t i = smoothD - 1; i < stochValues.size(); ++i)
    {
        double dSum = std::accumulate(stochValues.begin() + (i - smoothD + 1), stochValues.begin() + (i + 1), 0.0,
                                      [](double sum, const std::pair<double, double> &p)
                                      { return sum + p.first; });
        double smoothedD = dSum / smoothD;
        stochValues[i].second = smoothedD;
    }

    return stochValues;
}