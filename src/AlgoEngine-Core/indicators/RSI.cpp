#include "AlgoEngine-Core/algorithm/RSI.hpp"
#include <stdexcept>
#include <numeric>
#include <vector>
#include <algorithm>

SignalResult RSI::calculateSignal()
{
    if (historicalData.size() < period)
    {
        throw std::runtime_error("Not enough data to calculate RSI.");
    }

    double gain = 0.0;
    double loss = 0.0;

    for (size_t i = 1; i < historicalData.size(); ++i)
    {
        double change = historicalData[i].close - historicalData[i - 1].close;
        if (change > 0)
        {
            gain += change;
        }
        else
        {
            loss -= change;
        }
    }

    gain /= period;
    loss /= period;

    if (loss == 0)
    {
        return SignalResult(100.0);
    }

    double rs = gain / loss;
    double rsi = 100.0 - (100.0 / (1.0 + rs));

    signals.push_back(SignalResult(rsi));
    return signals.back();
}

std::vector<double> RSI::calculate(const std::vector<OHLCV> &historicalData, std::size_t period)
{
    if (historicalData.size() < period)
    {
        throw std::runtime_error("Not enough data to calculate RSI.");
    }

    std::vector<double> rsiValues;

    for (size_t i = period; i < historicalData.size(); ++i)
    {
        double gain = 0.0;
        double loss = 0.0;

        for (size_t j = i - period + 1; j <= i; ++j)
        {
            double change = historicalData[j].close - historicalData[j - 1].close;
            if (change > 0)
            {
                gain += change;
            }
            else
            {
                loss -= change;
            }
        }

        gain /= period;
        loss /= period;

        double rsi;
        if (loss == 0)
        {
            rsi = 100.0;
        }
        else
        {
            double rs = gain / loss;
            rsi = 100.0 - (100.0 / (1.0 + rs));
        }

        rsiValues.push_back(rsi);
    }

    return rsiValues;
}