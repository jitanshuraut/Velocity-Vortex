#include "AlgoEngine-Core/algorithm/MFI.hpp"
#include <stdexcept>
#include <numeric>

SignalResult MFI::calculateSignal()
{
    if (historicalData.size() < period)
    {
        throw std::runtime_error("Not enough data to calculate MFI.");
    }

    std::vector<double> mfiValues;
    double positiveMoneyFlow = 0.0;
    double negativeMoneyFlow = 0.0;

    for (size_t i = 1; i < period; ++i)
    {
        double typicalPriceCurrent = (historicalData[i].high + historicalData[i].low + historicalData[i].close) / 3.0;
        double typicalPricePrevious = (historicalData[i - 1].high + historicalData[i - 1].low + historicalData[i - 1].close) / 3.0;
        double moneyFlow = typicalPriceCurrent * historicalData[i].volume;

        if (typicalPriceCurrent > typicalPricePrevious)
            positiveMoneyFlow += moneyFlow;
        else if (typicalPriceCurrent < typicalPricePrevious)
            negativeMoneyFlow += moneyFlow;
    }

    if (negativeMoneyFlow == 0)
    {
        mfiValues.push_back(100.0);
    }
    else
    {
        double moneyFlowRatio = positiveMoneyFlow / negativeMoneyFlow;
        double mfi = 100 - (100 / (1 + moneyFlowRatio));
        mfiValues.push_back(mfi);
    }

    for (size_t i = period; i < historicalData.size(); ++i)
    {
        double typicalPriceCurrent = (historicalData[i].high + historicalData[i].low + historicalData[i].close) / 3.0;
        double typicalPricePrevious = (historicalData[i - 1].high + historicalData[i - 1].low + historicalData[i - 1].close) / 3.0;
        double moneyFlow = typicalPriceCurrent * historicalData[i].volume;

        // Update positive and negative money flows
        if (typicalPriceCurrent > typicalPricePrevious)
        {
            positiveMoneyFlow = (positiveMoneyFlow * (period - 1) + moneyFlow) / period;
            negativeMoneyFlow = (negativeMoneyFlow * (period - 1)) / period;
        }
        else if (typicalPriceCurrent < typicalPricePrevious)
        {
            negativeMoneyFlow = (negativeMoneyFlow * (period - 1) + moneyFlow) / period;
            positiveMoneyFlow = (positiveMoneyFlow * (period - 1)) / period;
        }

        double moneyFlowRatio = positiveMoneyFlow / (negativeMoneyFlow + 1e-10);
        double mfi = 100 - (100 / (1 + moneyFlowRatio));
        mfiValues.push_back(mfi);
    }

    signals.push_back(SignalResult(mfiValues));
    return SignalResult(mfiValues.back());
}

std::vector<double> MFI::calculate(const std::vector<OHLCV> &data, int period)
{
    if (data.size() < period)
        return {};

    std::vector<double> mfiValues;
    double positiveMoneyFlow = 0.0;
    double negativeMoneyFlow = 0.0;

    for (size_t i = 1; i < period; ++i)
    {
        double typicalPriceCurrent = (data[i].high + data[i].low + data[i].close) / 3.0;
        double typicalPricePrevious = (data[i - 1].high + data[i - 1].low + data[i - 1].close) / 3.0;
        double moneyFlow = typicalPriceCurrent * data[i].volume;

        if (typicalPriceCurrent > typicalPricePrevious)
            positiveMoneyFlow += moneyFlow;
        else if (typicalPriceCurrent < typicalPricePrevious)
            negativeMoneyFlow += moneyFlow;
    }

    if (negativeMoneyFlow == 0)
    {
        mfiValues.push_back(100.0);
    }
    else
    {
        double moneyFlowRatio = positiveMoneyFlow / negativeMoneyFlow;
        double mfi = 100 - (100 / (1 + moneyFlowRatio));
        mfiValues.push_back(mfi);
    }

    for (size_t i = period; i < data.size(); ++i)
    {
        double typicalPriceCurrent = (data[i].high + data[i].low + data[i].close) / 3.0;
        double typicalPricePrevious = (data[i - 1].high + data[i - 1].low + data[i - 1].close) / 3.0;
        double moneyFlow = typicalPriceCurrent * data[i].volume;

        if (typicalPriceCurrent > typicalPricePrevious)
        {
            positiveMoneyFlow = (positiveMoneyFlow * (period - 1) + moneyFlow) / period;
            negativeMoneyFlow = (negativeMoneyFlow * (period - 1)) / period;
        }
        else if (typicalPriceCurrent < typicalPricePrevious)
        {
            negativeMoneyFlow = (negativeMoneyFlow * (period - 1) + moneyFlow) / period;
            positiveMoneyFlow = (positiveMoneyFlow * (period - 1)) / period;
        }

        double moneyFlowRatio = positiveMoneyFlow / (negativeMoneyFlow + 1e-10);
        double mfi = 100 - (100 / (1 + moneyFlowRatio));
        mfiValues.push_back(mfi);
    }

    return mfiValues;
}