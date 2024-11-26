#include "AlgoEngine-Core/time_series_models/GARCH.hpp"
#include <iostream>
#include <numeric>

std::vector<double> GARCH::fitGARCH(const std::vector<double> &returns)
{

    if (returns.size() < p + q)
        throw std::invalid_argument("Insufficient data to fit GARCH model.");

    std::vector<double> conditionalVariances(returns.size(), 0.0);
    double omega = 0.1;
    double alphaSum = 0.7;
    double betaSum = 0.2;

    for (size_t t = 1; t < returns.size(); ++t)
    {
        conditionalVariances[t] = omega +
                                  alphaSum * (returns[t - 1] * returns[t - 1]) +
                                  betaSum * conditionalVariances[t - 1];
    }

    return conditionalVariances;
}

SignalResult GARCH::calculateSignal()
{
    if (historicalData.empty())
        return SignalResult(0.0);

    // Calculate returns from historical data
    std::vector<double> returns;
    for (size_t i = 1; i < historicalData.size(); ++i)
    {
        double returnVal = (historicalData[i].close - historicalData[i - 1].close) / historicalData[i - 1].close;
        returns.push_back(returnVal);
    }

    std::vector<double> conditionalVariances = fitGARCH(returns);

    double latestVariance = conditionalVariances.back();

    if (latestVariance > 0.01) // Example threshold
    {
        return SignalResult(latestVariance);
    }
    else
    {
        return SignalResult(-latestVariance);
    }
}
