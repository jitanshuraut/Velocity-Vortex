#include "AlgoEngine-Core/algorithm/MA.hpp"
#include <stdexcept>
#include <numeric>

SignalResult MA::calculateSignal()
{
    if (historicalData.size() < period)
    {
        throw std::runtime_error("Not enough data to calculate MA.");
    }

    double sum = std::accumulate(historicalData.end() - period, historicalData.end(), 0.0,
                                  [](double accumulator, const OHLCV &data) {
                                      return accumulator + data.close; 
                                  });
    double ma = sum / period;

    signals.push_back(SignalResult(ma)); 
    return SignalResult(ma); 
}

std::vector<double> MA::calculate(const std::vector<OHLCV> &data, int period)
{
    std::vector<double> maValues;
    if (data.size() < period)
        return maValues;

    for (size_t i = period - 1; i < data.size(); ++i)
    {
        double sum = 0.0;
        for (size_t j = 0; j < period; ++j)
        {
            sum += data[i - j].close; 
        }
        maValues.push_back(sum / period);
    }

    return maValues;
}