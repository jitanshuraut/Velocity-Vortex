#include "AlgoEngine-Core/algorithm/SMA.hpp"
#include <stdexcept>
#include <numeric>
#include <execution>

SignalResult SMA::calculateSignal()
{
    if (historicalData.size() < period)
    {
        throw std::runtime_error("Not enough data to calculate SMA.");
    }

    double sum = 0.0;
    size_t start = historicalData.size() - period;
    if (historicalData.size() > PARALLEL_THRESHOLD)
    {
        sum = std::transform_reduce(
            std::execution::par,
            historicalData.begin() + start,
            historicalData.end(),
            0.0,
            std::plus<>{},
            [](const OHLCV &data)
            { return data.close; });
    }
    else
    {
        for (size_t i = start; i < historicalData.size(); ++i)
        {
            sum += historicalData[i].close;
        }
    }

    double sma = sum / period;
    signals.push_back(SignalResult(sma));
    return signals.back();
}

std::vector<double> SMA::calculate(const std::vector<OHLCV> &historicalData, int period)
{
    std::vector<double> result;
    if (historicalData.size() < period)
    {
        throw std::runtime_error("Not enough data to calculate SMA.");
    }

    for (size_t i = period - 1; i < historicalData.size(); ++i)
    {
        double sum = 0.0;

        if (historicalData.size() > PARALLEL_THRESHOLD)
        {
            sum = std::transform_reduce(
                std::execution::par,
                historicalData.begin() + (i - period + 1),
                historicalData.begin() + i + 1,
                0.0,
                std::plus<>{},
                [](const OHLCV &data)
                { return data.close; });
        }
        else
        {
            for (size_t j = i - period + 1; j <= i; ++j)
            {
                sum += historicalData[j].close;
            }
        }
        result.push_back(sum / period);
    }
    return result;
}