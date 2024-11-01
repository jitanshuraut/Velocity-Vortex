#include "AlgoEngine-Core/statistical_models/BayesianMomentumTrading.hpp"
#include <cmath>
#include <numeric>

std::vector<double> BayesianMomentumTrading::calculateMomentum(const std::vector<OHLCV> &data, int period)
{
    std::vector<double> momentumProbabilities;

    if (data.size() < period)
        return momentumProbabilities;

    for (size_t i = period - 1; i < data.size(); ++i)
    {
        double prior = 0.5;
        double likelihoodUp = 1.0;
        double likelihoodDown = 1.0;

        for (size_t j = i - period + 1; j <= i; ++j)
        {
            double priceChange = data[j].close - data[j].open;
            likelihoodUp *= std::exp(priceChange / 100.0);
            likelihoodDown *= std::exp(-priceChange / 100.0);
        }

        double posteriorUp = (likelihoodUp * prior) / (likelihoodUp * prior + likelihoodDown * (1 - prior));
        momentumProbabilities.push_back(posteriorUp);
    }

    return momentumProbabilities;
}

SignalResult BayesianMomentumTrading::calculateSignal()
{
    if (historicalData.size() < period)
        return SignalResult(0.0);

    std::vector<double> momentumProbabilities = calculateMomentum(historicalData, period);

    if (!momentumProbabilities.empty())
    {
        return SignalResult(momentumProbabilities.back());
    }
    else
    {
        return SignalResult(0.0);
    }
}
