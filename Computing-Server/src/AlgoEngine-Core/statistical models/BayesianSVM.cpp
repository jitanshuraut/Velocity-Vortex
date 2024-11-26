#include "AlgoEngine-Core/statistical_models/BayesianSVM.hpp"
#include <cmath>
#include <numeric>
#include <algorithm>

std::vector<double> BayesianSVM::applySVM(const std::vector<OHLCV> &data, int period)
{
    std::vector<double> svmScores;

    if (data.size() < period)
        return svmScores;

    for (size_t i = period - 1; i < data.size(); ++i)
    {
        double svmScore = 0.0;
        double prior = 0.5; // Initial prior probability


        for (size_t j = i - period + 1; j <= i; ++j)
        {
            double priceChange = data[j].close - data[j].open;
            svmScore += priceChange;  
        }

        // Bayesian posterior adjustment
        double likelihoodPositive = std::exp(svmScore / 100.0); 
        double likelihoodNegative = std::exp(-svmScore / 100.0); 

        double posterior = (likelihoodPositive * prior) / 
                           (likelihoodPositive * prior + likelihoodNegative * (1 - prior));
        
        svmScores.push_back(posterior > 0.5 ? posterior : -posterior);  
    }

    return svmScores;
}

SignalResult BayesianSVM::calculateSignal()
{
    if (historicalData.size() < period)
        return SignalResult(0.0);

    std::vector<double> svmScores = applySVM(historicalData, period);

    if (!svmScores.empty())
    {
        return SignalResult(svmScores.back());  
    }
    else
    {
        return SignalResult(0.0); 
    }
}
