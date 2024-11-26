#include "AlgoEngine-Core/statistical_models/BayesianTrendDetection.hpp"

std::vector<double> BayesianTrendDetection::calculate(const std::vector<OHLCV> &data, int period)
{
    std::vector<double> trendProbabilities;

    if (data.size() < period)
        return trendProbabilities;

    for (size_t i = period - 1; i < data.size(); ++i)
    {
        double prior = 0.5; // Assuming an equal probability for trend and non-trend initially
        double likelihoodTrend = 1.0;
        double likelihoodNoTrend = 1.0;

        for (size_t j = i - period + 1; j <= i; ++j)
        {
            double priceChange = data[j].close - data[j].open;
            likelihoodTrend *= std::exp(priceChange / 100.0); 
            likelihoodNoTrend *= std::exp(-priceChange / 100.0);
        }

        double posteriorTrend = (likelihoodTrend * prior) / (likelihoodTrend * prior + likelihoodNoTrend * (1 - prior));
        trendProbabilities.push_back(posteriorTrend);
    }

    return trendProbabilities;
}

SignalResult BayesianTrendDetection::calculateSignal()
{
    if (historicalData.size() < period)
        return SignalResult(0.0); 

    std::vector<double> trendProbabilities = calculate(historicalData, period);

    if (!trendProbabilities.empty())
    {
        return SignalResult(trendProbabilities.back());
    }
    else
    {
        return SignalResult(0.0); 
    }
}
