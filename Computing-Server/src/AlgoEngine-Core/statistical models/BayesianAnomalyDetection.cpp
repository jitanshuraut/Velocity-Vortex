#include "AlgoEngine-Core/statistical_models/BayesianAnomalyDetection.hpp"
#include <cmath>
#include <numeric>

std::vector<double> BayesianAnomalyDetection::detectAnomalies(const std::vector<OHLCV> &data, int period)
{
    std::vector<double> anomalyScores;

    if (data.size() < period)
        return anomalyScores;

    for (size_t i = period - 1; i < data.size(); ++i)
    {
        double mean = std::accumulate(data.begin() + i - period + 1, data.begin() + i + 1, 0.0,
                                      [](double sum, const OHLCV &val)
                                      { return sum + val.close; }) /
                      period;

        double variance = 0.0;
        for (size_t j = i - period + 1; j <= i; ++j)
        {
            double diff = data[j].close - mean;
            variance += diff * diff;
        }
        variance /= period;

        double stdDev = std::sqrt(variance);
        double zScore = (data[i].close - mean) / stdDev;
        double anomalyProbability = std::exp(-0.5 * zScore * zScore) / (stdDev * std::sqrt(2 * M_PI));
        anomalyScores.push_back(1.0 - anomalyProbability); 
    }

    return anomalyScores;
}

SignalResult BayesianAnomalyDetection::calculateSignal()
{
    if (historicalData.size() < period)
        return SignalResult(0.0);

    std::vector<double> anomalyScores = detectAnomalies(historicalData, period);

    if (!anomalyScores.empty())
    {
        return SignalResult(anomalyScores.back());
    }
    else
    {
        return SignalResult(0.0);
    }
}
