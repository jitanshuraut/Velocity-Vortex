#pragma once

#include "../indicator/Indicator.hpp"
#include <vector>
#include <string>
#include <stdexcept>
#include "Utilities/SignalResult.hpp"

class GARCH : public Indicator
{
private:
    int p; // Order of the GARCH term
    int q; // Order of the ARCH term

public:
    GARCH(const std::string &indicatorName, int p, int q)
        : Indicator(indicatorName, 0), p(p), q(q) {}

    SignalResult calculateSignal() override;
    std::vector<double> fitGARCH(const std::vector<double> &returns);
};
