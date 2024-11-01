#include "AlgoEngine-Core/statistical_models/BayesianKernelRegression.hpp"
#include <cmath>
#include <numeric>

std::vector<double> BayesianKernelRegression::performRegression(const std::vector<OHLCV> &data, int period)
{
    std::vector<double> regressionResults;

    if (data.size() < period)
        return regressionResults;

    const double bandwidth = 1.0; 
    for (size_t i = period - 1; i < data.size(); ++i)
    {
        double weightedSum = 0.0;
        double weightTotal = 0.0;

        for (size_t j = i - period + 1; j <= i; ++j)
        {
            double distance = std::abs(static_cast<double>(data[i].timestamp - data[j].timestamp));
            double weight = std::exp(-distance * distance / (2 * bandwidth * bandwidth));

            weightedSum += weight * data[j].close;
            weightTotal += weight;
        }


        double prediction = weightedSum / weightTotal;
        double bayesianAdjustment = std::exp(-std::abs(data[i].close - prediction) / bandwidth);
        regressionResults.push_back(prediction * bayesianAdjustment);
    }

    return regressionResults;
}

SignalResult BayesianKernelRegression::calculateSignal()
{
    if (historicalData.size() < period)
        return SignalResult(0.0);

    std::vector<double> regressionResults = performRegression(historicalData, period);

    if (!regressionResults.empty())
    {
        return SignalResult(regressionResults.back());  
    }
    else
    {
        return SignalResult(0.0); 
    }
}
