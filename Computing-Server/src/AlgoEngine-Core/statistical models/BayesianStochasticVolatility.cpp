#include "AlgoEngine-Core/statistical_models/BayesianStochasticVolatility.hpp"
#include <stdexcept>

std::vector<double> BayesianStochasticVolatility::estimateVolatility(const std::vector<OHLCV> &data, int period)
{
    if (data.size() < period)
        throw std::invalid_argument("Insufficient data to estimate volatility.");

    std::vector<double> volatilities;

    for (size_t i = period; i < data.size(); ++i)
    {
        double sumReturns = 0.0;
        for (size_t j = i - period; j < i; ++j)
        {
            double dailyReturn = (data[j + 1].close - data[j].close) / data[j].close; 
            sumReturns += dailyReturn;
        }

        double meanReturn = sumReturns / period;
        double variance = 0.0;

        for (size_t j = i - period; j < i; ++j)
        {
            double dailyReturn = (data[j + 1].close - data[j].close) / data[j].close; 
            variance += std::pow(dailyReturn - meanReturn, 2);
        }

        variance /= period;
        volatilities.push_back(std::sqrt(variance)); 
    }

    return volatilities;
}

SignalResult BayesianStochasticVolatility::calculateSignal()
{
    if (historicalData.size() < period)
        return SignalResult(0.0);

    std::vector<double> volatilities = estimateVolatility(historicalData, period);
    double latestVolatility = volatilities.back();

    double threshold = 0.02; // Example threshold, can be adjusted

    if (latestVolatility > threshold)
    {
        return SignalResult(latestVolatility); 
    }
    else
    {
        return SignalResult(-latestVolatility); 
    }
}
