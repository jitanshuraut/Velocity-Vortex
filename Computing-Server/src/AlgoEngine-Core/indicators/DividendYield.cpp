#include "AlgoEngine-Core/algorithm/DividendYield.hpp"
#include <stdexcept>
#include <vector>

SignalResult DividendYield::calculateSignal()
{
    if (historicalData.empty())
    {
        throw std::runtime_error("Historical data is empty.");
    }

    if (dps <= 0)
    {
        throw std::runtime_error("Dividend per share must be greater than zero.");
    }

    signals.reserve(historicalData.size());
    for (const auto &bar : historicalData)
    {
        double dividendYield = dps / bar.close;
        signals.push_back(dividendYield);
    }

    return SignalResult(signals.back());
}

std::vector<double> DividendYield::calculate(const std::vector<OHLCV> &data, double dividendPerShare)
{
    if (data.empty() || dividendPerShare <= 0)
        return {};

    std::vector<double> dividendYields;
    dividendYields.reserve(data.size());

    for (const auto &bar : data)
    {
        double dividendYield = dividendPerShare / bar.close;
        dividendYields.push_back(dividendYield);
    }

    return dividendYields;
}