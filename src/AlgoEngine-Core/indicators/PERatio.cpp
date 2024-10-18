#include "AlgoEngine-Core/algorithm/PERatio.hpp"
#include <stdexcept>
#include <vector>

SignalResult PERatio::calculateSignal()
{
    if (historicalData.empty()) 
    {
        throw std::runtime_error("Historical data is empty.");
    }

    if (eps <= 0)
    {
        throw std::runtime_error("Earnings per share must be greater than zero.");
    }

    std::vector<double> peRatios;
    for (const auto &bar : historicalData) 
    {
        double peRatio = bar.close / eps;
        peRatios.push_back(peRatio);
        signals.push_back(SignalResult(peRatio)); 
    }

    return SignalResult(signals.back()); 
}

std::vector<double> PERatio::calculate(const std::vector<OHLCV> &data, double earningsPerShare)
{
    if (data.empty() || earningsPerShare <= 0)
        return {};

    std::vector<double> peRatios;
    for (const auto &bar : data)
    {
        double peRatio = bar.close / earningsPerShare;
        peRatios.push_back(peRatio);
    }

    return peRatios;
}