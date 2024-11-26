#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include "Utilities/OHLCV.hpp"
#include <variant>
#include "Utilities/SignalResult.hpp"

class Indicator
{
protected:
    std::vector<OHLCV> historicalData;
    std::vector<SignalResult> signals;
    int period;
    std::string name;

public:
    Indicator(const std::string &indicatorName, int period)
        : name(indicatorName), period(period) {}

    virtual ~Indicator() = default;

    void addData(const OHLCV &data)
    {
        historicalData.push_back(data);
    }

    virtual SignalResult calculateSignal() = 0;
    const std::vector<SignalResult> &getHistoricalSignals() const
    {
        return signals;
    }

    void reset()
    {
        historicalData.clear();
        signals.clear();
    }

    const std::string &getName() const { return name; }
    int getPeriod() const { return period; }
    int getDataSize() const { return historicalData.size(); }
    int getSignalSize() const { return signals.size(); }

    virtual void adjustParameters(int newPeriod)
    {
        period = newPeriod;
    }

    void backtest(const std::vector<OHLCV> &historicalPrices)
    {
        reset();
        for (const OHLCV &data : historicalPrices)
        {
            addData(data);
            SignalResult signal = calculateSignal();
            signals.push_back(signal);
        }
    }
};