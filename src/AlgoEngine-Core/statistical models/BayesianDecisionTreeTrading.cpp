#include "AlgoEngine-Core/statistical_models/BayesianDecisionTreeTrading.hpp"
#include <cmath>
#include <numeric>

std::vector<double> BayesianDecisionTreeTrading::evaluateDecisionTree(const std::vector<OHLCV> &data, int period)
{
    std::vector<double> decisionScores;

    if (data.size() < period)
        return decisionScores;

    for (size_t i = period - 1; i < data.size(); ++i)
    {
        // Simplified decision rules based on closing prices
        double upwardProbability = 0.5;
        double downwardProbability = 0.5;

        // Decision nodes
        for (size_t j = i - period + 1; j <= i; ++j)
        {
            if (data[j].close > data[j].open)
                upwardProbability *= 1.1;
            else
                downwardProbability *= 1.1;
        }

        double posteriorUp = upwardProbability / (upwardProbability + downwardProbability);
        double posteriorDown = downwardProbability / (upwardProbability + downwardProbability);

        decisionScores.push_back(posteriorUp - posteriorDown);
    }

    return decisionScores;
}

SignalResult BayesianDecisionTreeTrading::calculateSignal()
{
    if (historicalData.size() < period)
        return SignalResult(0.0);

    std::vector<double> decisionScores = evaluateDecisionTree(historicalData, period);

    if (!decisionScores.empty())
    {
        return SignalResult(decisionScores.back());
    }
    else
    {
        return SignalResult(0.0);
    }
}
