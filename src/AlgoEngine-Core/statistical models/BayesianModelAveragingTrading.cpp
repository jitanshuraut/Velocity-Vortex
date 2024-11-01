#include "AlgoEngine-Core/statistical_models/BayesianModelAveragingTrading.hpp"
#include <numeric>

double BayesianModelAveragingTrading::computeModelAverage(const std::vector<double> &modelPredictions, const std::vector<double> &weights)
{
    if (modelPredictions.size() != weights.size() || modelPredictions.empty())
        throw std::invalid_argument("Model predictions and weights must be the same size and not empty.");

    double weightedSum = 0.0;
    double totalWeight = 0.0;

    for (size_t i = 0; i < modelPredictions.size(); ++i)
    {
        weightedSum += modelPredictions[i] * weights[i];
        totalWeight += weights[i];
    }

    return weightedSum / totalWeight; // Calculate the weighted average
}

SignalResult BayesianModelAveragingTrading::calculateSignal()
{
    if (historicalData.size() < period)
        return SignalResult(0.0);


    std::vector<double> modelPredictions;
    std::vector<double> weights;

    for (size_t i = 0; i < historicalData.size() - period; ++i)
    {

        double model1Prediction = (historicalData[i + period - 1].close - historicalData[i].close) / historicalData[i].close;
        double model2Prediction = (historicalData[i + period - 1].volume - historicalData[i].volume) / historicalData[i].volume;

        modelPredictions.push_back(model1Prediction);
        modelPredictions.push_back(model2Prediction);

        weights.push_back(0.5); // Weight for model 1
        weights.push_back(0.5); // Weight for model 2
    }

    double averagedPrediction = computeModelAverage(modelPredictions, weights);

    if (averagedPrediction > 0)
    {
        return SignalResult(averagedPrediction); 
    }
    else
    {
        return SignalResult(-averagedPrediction); 
    }
}
