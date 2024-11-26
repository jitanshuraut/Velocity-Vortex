#include "Risk-Analysis-Core/VaRCalculator.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>

void VaRCalculator::set_historical_returns(const std::vector<double> &returns)
{
    historical_returns = returns;
}
void VaRCalculator::calculate_var()
{
    if (historical_returns.empty())
    {
        var_value = 0.0;
        return;
    }
    std::vector<double> sorted_returns = historical_returns;
    std::sort(sorted_returns.begin(), sorted_returns.end());

    size_t index = static_cast<size_t>((1.0 - confidence_level) * sorted_returns.size());
    var_value = -sorted_returns[index];
}

double VaRCalculator::get_var()
{
    return var_value;
}