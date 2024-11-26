#include "AlgoEngine-Core/algorithm/BollingerBands.hpp"
#include <cmath>
#include <stdexcept>
#include <numeric>
#include <vector>
#include <algorithm>

SignalResult BollingerBands::calculateSignal()
{
    if (historicalData.size() < period)
    {
        throw std::runtime_error("Not enough data to calculate Bollinger Bands.");
    }

    auto [upperBand, lowerBand] = calculate();

    signals.push_back(upperBand);
    signals.push_back(lowerBand);

    return SignalResult(0); 
}

std::pair<double, double> BollingerBands::calculate() const
{
    if (historicalData.size() < period)
    {
        throw std::runtime_error("Not enough data to calculate Bollinger Bands.");
    }
    double sum = std::accumulate(
        historicalData.end() - period, historicalData.end(), 0.0,
        [](double acc, const OHLCV& data) { return acc + data.close; });

    double sq_sum = std::accumulate(
        historicalData.end() - period, historicalData.end(), 0.0,
        [](double acc, const OHLCV& data) { return acc + data.close * data.close; });

    double mean = sum / period;
    double variance = (sq_sum / period) - (mean * mean);
    double stdDev = std::sqrt(variance);

    double upperBand = mean + numStdDev * stdDev;
    double lowerBand = mean - numStdDev * stdDev;

    return {upperBand, lowerBand};
}

std::pair<double, double> BollingerBands::calculate(const std::vector<double> &prices, int period, double numStdDev)
{
    if (prices.size() < period)
    {
        throw std::runtime_error("Not enough data to calculate Bollinger Bands.");
    }
    double sum = std::accumulate(
        prices.end() - period, prices.end(), 0.0);

    double sq_sum = std::accumulate(
        prices.end() - period, prices.end(), 0.0,
        [](double acc, double price) { return acc + price * price; });

    double mean = sum / period;
    double variance = (sq_sum / period) - (mean * mean);
    double stdDev = std::sqrt(variance);

    double upperBand = mean + numStdDev * stdDev;
    double lowerBand = mean - numStdDev * stdDev;

    return {upperBand, lowerBand};
}