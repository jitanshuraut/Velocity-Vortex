#include "AlgoEngine-Core/algorithm/EMA.hpp"
#include <stdexcept>
#include <numeric>
#include <vector>

SignalResult EMA::calculateSignal()
{
    if (historicalData.empty())
    {
        throw std::runtime_error("Not enough data to calculate EMA.");
    }

    std::vector<double> ema(historicalData.size(), 0.0);
    double multiplier = 2.0 / (period + 1);
    double sum = 0.0;
    for (std::size_t i = 0; i < period && i < historicalData.size(); ++i)
    {
        sum += historicalData[i].close;
    }

    if (period > 0 && historicalData.size() > 0)
    {
        ema[period - 1] = sum / period;
    }
    for (std::size_t i = period; i < historicalData.size(); ++i)
    {
        ema[i] = ((historicalData[i].close - ema[i - 1]) * multiplier) + ema[i - 1];
    }

    signals.push_back(SignalResult(ema));
    return SignalResult(ema.back());
}

std::vector<double> EMA::calculate(const std::vector<OHLCV> &data, std::size_t period)
{
    if (data.empty())
        return {};

    std::vector<double> ema(data.size(), 0.0);
    double multiplier = 2.0 / (period + 1);

    double sum = 0.0;
    for (std::size_t i = 0; i < period && i < data.size(); ++i)
    {
        sum += data[i].close;
    }

    if (period > 0 && data.size() > 0)
    {
        ema[period - 1] = sum / period;
    }

    for (std::size_t i = period; i < data.size(); ++i)
    {
        ema[i] = ((data[i].close - ema[i - 1]) * multiplier) + ema[i - 1];
    }

    return ema;
}