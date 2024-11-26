#include "AlgoEngine-Core/time_series_models/SARIMA.hpp"
#include <numeric>

std::vector<double> SARIMA::fitSARIMA(const std::vector<OHLCV> &data, int p, int d, int q, int P, int D, int Q, int s)
{
    if (data.size() < p + d + q + P + D + Q + s)
        throw std::invalid_argument("Insufficient data to fit SARIMA model.");

    std::vector<double> differencedData;
    std::vector<double> seasonalDifferencedData;
    std::vector<double> predictions;

    for (size_t i = d; i < data.size(); ++i)
    {
        differencedData.push_back(data[i].close - data[i - d].close);
    }

    for (size_t i = s + D; i < differencedData.size(); ++i)
    {
        seasonalDifferencedData.push_back(differencedData[i] - seasonalDifferencedData[i - s]);
    }

    std::vector<double> arTerms(p, 0.0);
    std::vector<double> maTerms(q, 0.0);

    for (size_t i = p; i < seasonalDifferencedData.size(); ++i)
    {
        double arSum = 0.0;
        for (int j = 1; j <= p; ++j)
        {
            arSum += arTerms[j - 1] * seasonalDifferencedData[i - j];
        }
        arTerms.push_back(arSum);
    }

    for (size_t i = q; i < seasonalDifferencedData.size(); ++i)
    {
        double maSum = 0.0;
        for (int j = 1; j <= q; ++j)
        {
            maSum += maTerms[j - 1] * (seasonalDifferencedData[i - j] - arTerms[i - j]);
        }
        maTerms.push_back(maSum);
    }

    for (size_t i = 0; i < seasonalDifferencedData.size(); ++i)
    {
        predictions.push_back(arTerms[i] + maTerms[i]);
    }

    return predictions;
}

SignalResult SARIMA::calculateSignal()
{
    if (historicalData.size() < p + d + q + P + D + Q + s)
        return SignalResult(0.0);

    std::vector<double> predictions = fitSARIMA(historicalData, p, d, q, P, D, Q, s);
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
