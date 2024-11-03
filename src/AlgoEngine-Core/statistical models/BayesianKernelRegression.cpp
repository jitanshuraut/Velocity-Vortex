#include "AlgoEngine-Core/statistical_models/BayesianKernelRegression.hpp"
#include <cmath>
#include <numeric>

std::chrono::system_clock::time_point BayesianKernelRegression::parseTimestamp(const std::string &timestamp)
{
    std::istringstream ss(timestamp);
    std::tm t = {};
    ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");
    if (ss.fail())
    {
        throw std::runtime_error("Failed to parse timestamp");
    }
    return std::chrono::system_clock::from_time_t(std::mktime(&t));
}

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

            auto time1 = parseTimestamp(data[i].timestamp);
            auto time2 = parseTimestamp(data[j].timestamp);
            auto diff = std::chrono::duration<double>(time1 - time2).count();

            double distance = std::abs(diff);
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
