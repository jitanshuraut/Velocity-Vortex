#include "AlgoEngine-Core/algorithm/APO.hpp"
#include <stdexcept>
#include <numeric>
#include <cmath>

SignalResult APO::calculateSignal()
{

    if (historicalData.size() < period || historicalData.size() < longPeriod)
    {
        throw std::runtime_error("Not enough data to calculate APO.");
    }

    double shortMultiplier = 2.0 / (period + 1);
    double shortEMA = historicalData.back().close;
    for (size_t i = historicalData.size() - period; i < historicalData.size(); ++i)
    {
        shortEMA = ((historicalData[i].close - shortEMA) * shortMultiplier) + shortEMA;
    }

    double longMultiplier = 2.0 / (longPeriod + 1);
    double longEMA = historicalData.back().close;
    for (size_t i = historicalData.size() - longPeriod; i < historicalData.size(); ++i)
    {
        longEMA = ((historicalData[i].close - longEMA) * longMultiplier) + longEMA;
    }

    double apoValue = shortEMA - longEMA;
    signals.push_back(apoValue);
    return SignalResult(signals.back());
}

std::vector<double> APO::calculate(const std::vector<OHLCV> &ohlcvData, int shortPeriod, int longPeriod)
{
    std::vector<double> apoValues;

    if (ohlcvData.size() < std::max(shortPeriod, longPeriod))
    {
        throw std::runtime_error("Not enough data to calculate APO.");
    }

    std::vector<double> shortEMA(ohlcvData.size(), 0.0);
    std::vector<double> longEMA(ohlcvData.size(), 0.0);

    double shortMultiplier = 2.0 / (shortPeriod + 1);
    shortEMA[0] = ohlcvData[0].close;
    for (size_t i = 1; i < ohlcvData.size(); ++i)
    {
        shortEMA[i] = ((ohlcvData[i].close - shortEMA[i - 1]) * shortMultiplier) + shortEMA[i - 1];
    }

    double longMultiplier = 2.0 / (longPeriod + 1);
    longEMA[0] = ohlcvData[0].close;
    for (size_t i = 1; i < ohlcvData.size(); ++i)
    {
        longEMA[i] = ((ohlcvData[i].close - longEMA[i - 1]) * longMultiplier) + longEMA[i - 1];
    }

    for (size_t i = std::max(shortPeriod, longPeriod) - 1; i < ohlcvData.size(); ++i)
    {
        double apoValue = shortEMA[i] - longEMA[i];
        apoValues.push_back(apoValue);
    }

    return apoValues;
}