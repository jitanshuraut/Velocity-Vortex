#include "Risk-Analysis-Core/RiskAdjustedMetrics.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

double RiskAdjustedMetrics::calculate_standard_deviation(const std::vector<double> &returns)
{
    double mean = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    double sum_squared_diff = 0.0;

    for (double return_value : returns)
    {
        sum_squared_diff += (return_value - mean) * (return_value - mean);
    }

    return std::sqrt(sum_squared_diff / returns.size());
}

double RiskAdjustedMetrics::calculate_max_drawdown(const std::vector<double> &returns)
{
    double max_drawdown = 0.0;
    double peak = returns[0];

    for (double return_value : returns)
    {
        if (return_value > peak)
        {
            peak = return_value;
        }
        double drawdown = (peak - return_value) / peak;
        max_drawdown = std::max(max_drawdown, drawdown);
    }

    return max_drawdown;
}

void RiskAdjustedMetrics::set_total_return(double return_value)
{
    total_return = return_value;
}

void RiskAdjustedMetrics::set_total_risk(double risk_value)
{
    total_risk = risk_value;
}

double RiskAdjustedMetrics::calculate_sharpe_ratio(double risk_free_rate)
{
    return (total_return - risk_free_rate) / total_risk;
}

double RiskAdjustedMetrics::calculate_sortino_ratio(const std::vector<double> &returns, double target_return)
{
    double downside_deviation = 0.0;
    int count = 0;

    for (double return_value : returns)
    {
        if (return_value < target_return)
        {
            downside_deviation += (return_value - target_return) * (return_value - target_return);
            count++;
        }
    }

    if (count > 0)
    {
        downside_deviation = std::sqrt(downside_deviation / count);
        return (total_return - target_return) / downside_deviation;
    }

    return 0.0;
}

double RiskAdjustedMetrics::calculate_calmar_ratio(const std::vector<double> &returns)
{
    double max_drawdown = calculate_max_drawdown(returns);
    if (max_drawdown > 0)
    {
        return total_return / max_drawdown;
    }
    return 0.0;
}

double RiskAdjustedMetrics::calculate_information_ratio(double benchmark_return)
{
    return (total_return - benchmark_return) / total_risk;
}

double RiskAdjustedMetrics::calculate_treynor_ratio(double risk_free_rate, double portfolio_beta)
{
    return (total_return - risk_free_rate) / portfolio_beta;
}