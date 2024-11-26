#include "AlgoEngine-Core/algorithm/VWAP.hpp"
#include <stdexcept>
#include <vector>
#include <numeric>

SignalResult VWAP::calculateSignal()
{
    if (historicalData.empty())
    {
        throw std::runtime_error("Historical data is empty.");
    }

    std::vector<double> vwapValues;
    double cumulativeVolume = 0.0;
    double cumulativePriceVolume = 0.0;

    for (const auto &bar : historicalData)
    {
        double priceVolume = bar.close * bar.volume;
        cumulativeVolume += bar.volume;
        cumulativePriceVolume += priceVolume;

        double vwap = cumulativePriceVolume / cumulativeVolume;
        vwapValues.push_back(vwap);
        signals.push_back(SignalResult(vwap));
    }

    return SignalResult(vwapValues);
}

std::vector<double> VWAP::calculate(const std::vector<OHLCV> &data)
{
    if (data.empty())
        return {};

    std::vector<double> vwapValues;
    double cumulativeVolume = 0.0;
    double cumulativePriceVolume = 0.0;

    for (const auto &bar : data)
    {
        double priceVolume = bar.close * bar.volume;
        cumulativeVolume += bar.volume;
        cumulativePriceVolume += priceVolume;

        double vwap = cumulativePriceVolume / cumulativeVolume;
        vwapValues.push_back(vwap);
    }

    return vwapValues;
}