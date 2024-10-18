#include "AlgoEngine-Core/algorithm/OBV.hpp"
#include <stdexcept>

SignalResult OBV::calculateSignal()
{
    if (historicalData.empty())
    {
        throw std::runtime_error("No historical data available to calculate OBV.");
    }

    std::vector<double> obvValues; 
    double obv = 0.0;              

    for (size_t i = 1; i < historicalData.size(); ++i)
    {
        if (historicalData[i].close > historicalData[i - 1].close)
        {
            obv += historicalData[i].volume; 
        }
        else if (historicalData[i].close < historicalData[i - 1].close)
        {
            obv -= historicalData[i].volume; 
        }
        obvValues.push_back(obv); 
    }

    signals.push_back(SignalResult(obvValues)); 
    return SignalResult(signals.back());        
}

std::vector<double> OBV::calculate(const std::vector<OHLCV> &data)
{
    std::vector<double> obvValues;
    if (data.empty())
        return obvValues;

    double obv = 0.0; 

    for (size_t i = 1; i < data.size(); ++i)
    {
        if (data[i].close > data[i - 1].close)
            obv += data[i].volume; 
        else if (data[i].close < data[i - 1].close)
            obv -= data[i].volume; 

        obvValues.push_back(obv); 
    }

    return obvValues;
}