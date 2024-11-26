#include "AlgoEngine-Core/statistical_models/BayesianCorrelationEstimation.hpp"
#include <cmath>
#include <numeric>

std::vector<double> BayesianCorrelationEstimation::calculate(const std::vector<OHLCV> &data1, const std::vector<OHLCV> &data2, int period)
{
    std::vector<double> correlations;

    if (data1.size() < period || data2.size() < period)
        return correlations;

    for (size_t i = period - 1; i < data1.size(); ++i)
    {
        double mean1 = std::accumulate(data1.begin() + i - period + 1, data1.begin() + i + 1, 0.0,
                                        [](double sum, const OHLCV &val) { return sum + val.close; }) / period;
        double mean2 = std::accumulate(data2.begin() + i - period + 1, data2.begin() + i + 1, 0.0,
                                        [](double sum, const OHLCV &val) { return sum + val.close; }) / period;

 
        double covariance = 0.0, variance1 = 0.0, variance2 = 0.0;
        for (size_t j = i - period + 1; j <= i; ++j)
        {
            double diff1 = data1[j].close - mean1;
            double diff2 = data2[j].close - mean2;
            covariance += diff1 * diff2;
            variance1 += diff1 * diff1;
            variance2 += diff2 * diff2;
        }

        covariance /= period;
        variance1 = std::sqrt(variance1 / period);
        variance2 = std::sqrt(variance2 / period);
        double correlation = (covariance / (variance1 * variance2));
        correlations.push_back(correlation);
    }

    return correlations;
}

SignalResult BayesianCorrelationEstimation::calculateSignal()
{
    if (historicalData.size() < period)
        return SignalResult(0.0);

    std::vector<double> correlationEstimates = calculate(historicalData, historicalData, period);

    if (!correlationEstimates.empty())
    {
        return SignalResult(correlationEstimates.back());
    }
    else
    {
        return SignalResult(0.0);
    }
}
