#include "AlgoEngine-Core/statistical_models/BayesianGARCHTrading.hpp"
#include <numeric>

std::vector<double> BayesianGARCHTrading::estimateGARCH(const std::vector<OHLCV> &data, int period)
{
    if (data.size() < period)
        throw std::invalid_argument("Insufficient data to estimate GARCH model.");

    std::vector<double> volatilities;
    double omega = 0.1; // Constant term
    double alpha = 0.2;
    double beta = 0.7;

    double lastVariance = data[0].close * data[0].close;

    for (size_t i = 1; i < data.size(); ++i)
    {
        double return_t = (data[i].close - data[i - 1].close) / data[i - 1].close;
        lastVariance = omega + alpha * return_t * return_t + beta * lastVariance;
        volatilities.push_back(std::sqrt(lastVariance));
    }

    return volatilities;
}

SignalResult BayesianGARCHTrading::calculateSignal()
{
    if (historicalData.size() < period)
        return SignalResult(0.0);

    std::vector<double> volatilities = estimateGARCH(historicalData, period);

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
