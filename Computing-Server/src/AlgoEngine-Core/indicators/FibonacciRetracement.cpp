#include "AlgoEngine-Core/algorithm/FibonacciRetracement.hpp"
#include <stdexcept>
#include <algorithm>

SignalResult FibonacciRetracement::calculateSignal()
{
    if (historicalData.empty())
    {
        throw std::runtime_error("Historical data is empty.");
    }

    double maxPrice = historicalData[0].high;
    double minPrice = historicalData[0].low;

    for (const auto &bar : historicalData)
    {
        if (bar.high > maxPrice) maxPrice = bar.high;
        if (bar.low < minPrice) minPrice = bar.low;
    }

    double difference = maxPrice - minPrice;
    std::vector<double> levels = {
        maxPrice,
        maxPrice - difference * 0.236,
        maxPrice - difference * 0.382,
        maxPrice - difference * 0.618,
        minPrice
    };

    signals.push_back(SignalResult(levels));
    return SignalResult(signals.back()); 
}

std::vector<double> FibonacciRetracement::calculate(const std::vector<OHLCV> &data)
{
    if (data.empty()) return {};

    double maxPrice = data[0].high;
    double minPrice = data[0].low;

    for (const auto &bar : data)
    {
        if (bar.high > maxPrice) maxPrice = bar.high;
        if (bar.low < minPrice) minPrice = bar.low;
    }

    double difference = maxPrice - minPrice;
    std::vector<double> levels = {
        maxPrice,
        maxPrice - difference * 0.236,
        maxPrice - difference * 0.382,
        maxPrice - difference * 0.618,
        minPrice
    };

    return levels;
}