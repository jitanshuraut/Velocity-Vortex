#include "AlgoEngine-Core/statistical_models/BayesianCausalInferenceTrading.hpp"
#include <cmath>
#include <numeric>
#include <algorithm>

std::map<std::string, double> BayesianCausalInferenceTrading::performCausalInference(const std::vector<OHLCV> &data, int period)
{
    std::map<std::string, double> causalEffects;

    if (data.size() < period)
        return causalEffects;

    double totalUpMoves = 0, totalDownMoves = 0;
    double totalVolumeIncrease = 0, totalVolumeDecrease = 0;

    for (size_t i = 1; i < data.size(); ++i)
    {
        if (data[i].close > data[i - 1].close)
        {
            totalUpMoves++;
            if (data[i].volume > data[i - 1].volume)
                totalVolumeIncrease++;
        }
        else
        {
            totalDownMoves++;
            if (data[i].volume < data[i - 1].volume)
                totalVolumeDecrease++;
        }
    }

    double pUpGivenVolumeIncrease = totalVolumeIncrease / totalUpMoves;
    double pDownGivenVolumeDecrease = totalVolumeDecrease / totalDownMoves;

    causalEffects["P(Up|VolumeIncrease)"] = pUpGivenVolumeIncrease;     // P(Price Up | Volume Increase)
    causalEffects["P(Down|VolumeDecrease)"] = pDownGivenVolumeDecrease; // P(Price Down | Volume Decrease)

    return causalEffects;
}

SignalResult BayesianCausalInferenceTrading::calculateSignal()
{
    if (historicalData.size() < period)
        return SignalResult(0.0);

    auto causalEffects = performCausalInference(historicalData, period);

    if (causalEffects["P(Up|VolumeIncrease)"] > causalEffects["P(Down|VolumeDecrease)"])
    {
        return SignalResult(causalEffects["P(Up|VolumeIncrease"]);
    }
    else
    {
        return SignalResult(-causalEffects["P(Down|VolumeDecrease"]);
    }
}
