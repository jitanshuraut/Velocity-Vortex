#include "AlgoEngine-Core/algorithm/ATR.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

SignalResult ATR::calculateSignal()
{

    if (historicalData.size() < 2)
    {
        throw std::runtime_error("Not enough data to calculate ATR.");
    }

    double high = historicalData[historicalData.size() - 2].high;
    double low = historicalData[historicalData.size() - 2].low;
    double close = historicalData.back().close;

    double tr = std::max({high - low, std::abs(high - previousClose), std::abs(low - previousClose)});
    previousClose = close;

    if (signals.empty())
    {
        signals.push_back(tr);
    }
    else
    {
        double atr = (signals.back().getSingleValue() * (period - 1) + tr) / period;
        signals.push_back(atr);
    }

    return SignalResult(signals.back());
}

std::vector<double> ATR::calculate(const std::vector<OHLCV> &ohlcvData, int period)
{
    if (ohlcvData.size() < period)
    {
        throw std::runtime_error("Not enough data to calculate ATR.");
    }

    std::vector<double> atrValues;
    std::vector<double> trueRanges;

    for (size_t i = 1; i < ohlcvData.size(); ++i)
    {
        double tr = std::max({ohlcvData[i].high - ohlcvData[i].low,
                              std::abs(ohlcvData[i].high - ohlcvData[i - 1].close),
                              std::abs(ohlcvData[i].low - ohlcvData[i - 1].close)});
        trueRanges.push_back(tr);
    }

    double sum = 0.0;
    for (size_t i = 0; i < trueRanges.size(); ++i)
    {
        if (i < period)
        {
            sum += trueRanges[i];
            if (i == period - 1)
            {
                atrValues.push_back(sum / period);
            }
        }
        else
        {
            double atr = (atrValues.back() * (period - 1) + trueRanges[i]) / period;
            atrValues.push_back(atr);
        }
    }

    return atrValues;
}