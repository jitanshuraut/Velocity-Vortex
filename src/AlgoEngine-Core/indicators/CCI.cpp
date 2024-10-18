#include "AlgoEngine-Core/algorithm/CCI.hpp"
#include <stdexcept>
#include <numeric>
#include <thread>
#include <future>
#include <cmath>

SignalResult CCI::calculateSignal()
{
    if (historicalData.size() < period)
    {
        throw std::runtime_error("Not enough data to calculate CCI.");
    }

    const double constant = 0.015;
    std::vector<double> cciValues(historicalData.size() - period, 0.0);
    size_t dataSize = historicalData.size();
    size_t numThreads = std::thread::hardware_concurrency();
    size_t chunkSize = (dataSize - period) / numThreads;

    std::vector<std::future<void>> futures;

    for (size_t i = 0; i < numThreads; ++i)
    {
        size_t start = i * chunkSize;
        size_t end = (i == numThreads - 1) ? (dataSize - period) : (start + chunkSize);

        if (start >= end)
        {
            break;
        }

        futures.emplace_back(std::async(std::launch::async, &CCI::calculateCCIRange, this, start, end, std::ref(historicalData), constant, std::ref(cciValues)));
    }

    for (auto &future : futures)
    {
        future.get();
    }

    for (const auto &value : cciValues)
    {
        if (value != 0.0)
        {
            signals.push_back(value);
        }
    }

    return SignalResult(cciValues.back());
}

void CCI::calculateCCIRange(size_t start, size_t end, const std::vector<OHLCV> &data, double constant, std::vector<double> &cciValues)
{
    for (size_t i = start; i < end; ++i)
    {
        double meanTypicalPrice = 0.0;
        double meanDeviation = 0.0;

        for (size_t j = i; j < i + period; ++j)
        {
            if (j >= data.size())
            {
                return;
            }

            double typicalPrice = (data[j].high + data[j].low + data[j].close) / 3.0;
            meanTypicalPrice += typicalPrice;
            if (j == i)
                meanDeviation = typicalPrice;
            else
                meanDeviation += std::abs(typicalPrice - meanDeviation);
        }

        meanTypicalPrice /= period;
        meanDeviation /= period;

        if (meanDeviation != 0.0)
        {
            double cci = (meanTypicalPrice - meanDeviation) / (constant * meanDeviation);
            cciValues[i] = cci;
        }
        else
        {
            cciValues[i] = 0.0;
        }
    }
}

std::vector<double> CCI::calculate(const std::vector<OHLCV> &data, int period)
{
    if (data.size() < period)
        throw std::runtime_error("Not enough data to calculate CCI.");

    const double constant = 0.015;
    std::vector<double> cciValues(data.size() - period, 0.0);
    size_t dataSize = data.size();
    size_t numThreads = std::thread::hardware_concurrency();
    size_t chunkSize = (dataSize - period) / numThreads;

    std::vector<std::future<void>> futures;

    for (size_t i = 0; i < numThreads; ++i)
    {
        size_t start = i * chunkSize;
        size_t end = (i == numThreads - 1) ? (dataSize - period) : (start + chunkSize);

        if (start >= end)
        {
            break;
        }

        futures.emplace_back(std::async(std::launch::async, &CCI::calculateCCIRange, this, start, end, std::ref(data), constant, std::ref(cciValues)));
    }

    for (auto &future : futures)
    {
        future.get();
    }
    return cciValues;
}