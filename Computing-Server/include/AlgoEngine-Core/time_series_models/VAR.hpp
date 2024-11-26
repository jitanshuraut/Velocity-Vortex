#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>
#include <string>
#include <stdexcept>
#include "Utilities/SignalResult.hpp"

class VAR : public Indicator
{
private:
    int lagOrder; 

public:
    VAR(const std::string &indicatorName, int lagOrder)
        : Indicator(indicatorName, lagOrder), lagOrder(lagOrder) {}

    SignalResult calculateSignal() override;
    std::vector<std::vector<double>> fitVAR(const std::vector<std::vector<OHLCV>> &data);
};
