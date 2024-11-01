#include "AlgoEngine-Core/time_series_models/VAR.hpp"
#include <numeric>
#include <iostream>

std::vector<std::vector<double>> VAR::fitVAR(const std::vector<std::vector<OHLCV>> &data)
{
    if (data.size() < lagOrder + 1)
        throw std::invalid_argument("Insufficient data to fit VAR model.");

    std::vector<std::vector<double>> predictions(data[0].size(), std::vector<double>(data.size()));
    for (size_t i = lagOrder; i < data[0].size(); ++i)
    {
        for (size_t j = 0; j < data.size(); ++j)
        {
            predictions[j][i] = data[j][i].close;
        }
    }

    return predictions;
}

SignalResult VAR::calculateSignal()
{
    if (historicalData.empty())
        return SignalResult(0.0);

    std::vector<std::vector<OHLCV>> multiData;
    for (const OHLCV &ohlcv : historicalData)
    {
        multiData.push_back({ohlcv});
    }

    std::vector<std::vector<double>> predictions = fitVAR(multiData);

    double latestPrediction = predictions[0].back();

    if (latestPrediction > historicalData.back().close)
    {
        return SignalResult(latestPrediction);
    }
    else
    {
        return SignalResult(-latestPrediction);
    }
}
