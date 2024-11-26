#include "AlgoEngine-Core/algorithm/ADX.hpp"
#include <stdexcept>
#include <numeric>
#include <cmath>

SignalResult ADX::calculateSignal()
{
    if (historicalData.size() < period)
    {
        throw std::runtime_error("Not enough data to calculate ADX.");
    }

    double plusDM = 0.0;
    double minusDM = 0.0;
    double trueRange = 0.0;

    for (size_t i = 1; i < historicalData.size(); ++i)
    {
        double highDiff = historicalData[i].high - historicalData[i - 1].high;
        double lowDiff = historicalData[i - 1].low - historicalData[i].low;

        plusDM += (highDiff > lowDiff && highDiff > 0) ? highDiff : 0.0;
        minusDM += (lowDiff > highDiff && lowDiff > 0) ? -lowDiff : 0.0;

        double tr1 = historicalData[i].high - historicalData[i].low;
        double tr2 = std::abs(historicalData[i].high - historicalData[i - 1].close);
        double tr3 = std::abs(historicalData[i].low - historicalData[i - 1].close);
        trueRange += std::max({tr1, tr2, tr3});
    }

    double smoothedPlusDM = plusDM / period;
    double smoothedMinusDM = minusDM / period;
    double smoothedTrueRange = trueRange / period;

    double plusDI = (smoothedPlusDM / smoothedTrueRange) * 100;
    double minusDI = (smoothedMinusDM / smoothedTrueRange) * 100;

    double adx = (plusDI + minusDI == 0) ? 0 : std::abs((plusDI - minusDI) / (plusDI + minusDI)) * 100;
    signals.push_back(adx);

    return SignalResult(signals.back());
}

std::vector<double> ADX::calculate(const std::vector<OHLCV> &data, int period)
{
    std::vector<double> adxValues;
    if (data.size() < period)
        return adxValues;

    double plusDM = 0.0;
    double minusDM = 0.0;
    double trueRange = 0.0;

    for (size_t i = 1; i < data.size(); ++i)
    {
        double highDiff = data[i].high - data[i - 1].high;
        double lowDiff = data[i - 1].low - data[i].low;

        plusDM += (highDiff > lowDiff && highDiff > 0) ? highDiff : 0.0;
        minusDM += (lowDiff > highDiff && lowDiff > 0) ? -lowDiff : 0.0;

        double tr1 = data[i].high - data[i].low;
        double tr2 = std::abs(data[i].high - data[i - 1].close);
        double tr3 = std::abs(data[i].low - data[i - 1].close);
        trueRange += std::max({tr1, tr2, tr3});
    }

    double smoothedPlusDM = plusDM / period;
    double smoothedMinusDM = minusDM / period;
    double smoothedTrueRange = trueRange / period;

    double plusDI = (smoothedPlusDM / smoothedTrueRange) * 100;
    double minusDI = (smoothedMinusDM / smoothedTrueRange) * 100;

    double adx = (plusDI + minusDI == 0) ? 0 : std::abs((plusDI - minusDI) / (plusDI + minusDI)) * 100;
    adxValues.push_back(adx);

    return adxValues;
}