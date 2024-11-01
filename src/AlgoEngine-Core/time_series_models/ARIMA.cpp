#include "AlgoEngine-Core/time_series_models/ARIMA.hpp"
#include <numeric>

std::vector<double> ARIMA::fitARIMA(const std::vector<OHLCV> &data, int p, int d, int q)
{
    if (data.size() < p + d + q)
        throw std::invalid_argument("Insufficient data to fit ARIMA model.");

    std::vector<double> differencedData;
    std::vector<double> predictions;

    for (size_t i = d; i < data.size(); ++i)
    {
        differencedData.push_back(data[i].close - data[i - d].close);
    }

    std::vector<double> arTerms(p, 0.0);
    std::vector<double> maTerms(q, 0.0);

    for (size_t i = p; i < differencedData.size(); ++i)
    {
        double arSum = 0.0;
        for (int j = 1; j <= p; ++j)
        {
            arSum += arTerms[j - 1] * differencedData[i - j];
        }
        arTerms.push_back(arSum);
    }

    for (size_t i = q; i < differencedData.size(); ++i)
    {
        double maSum = 0.0;
        for (int j = 1; j <= q; ++j)
        {
            maSum += maTerms[j - 1] * (differencedData[i - j] - arTerms[i - j]);
        }
        maTerms.push_back(maSum);
    }

    for (size_t i = 0; i < differencedData.size(); ++i)
    {
        predictions.push_back(arTerms[i] + maTerms[i]);
    }

    return predictions;
}

SignalResult ARIMA::calculateSignal()
{
    if (historicalData.size() < p + d + q)
        return SignalResult(0.0);

    std::vector<double> predictions = fitARIMA(historicalData, p, d, q);
    double latestPrediction = predictions.back();

    if (latestPrediction > historicalData.back().close)
    {
        return SignalResult(latestPrediction);
    }
    else
    {
        return SignalResult(-latestPrediction);
    }
}
