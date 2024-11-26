#ifndef RISKADJUSTEDMETRICS_HPP
#define RISKADJUSTEDMETRICS_HPP

#include <vector>

class RiskAdjustedMetrics
{
private:
    double total_return;
    double total_risk;
    double sharpe_ratio;
    double sortino_ratio;
    double calmar_ratio;
    double information_ratio;
    double treynor_ratio;

    double calculate_max_drawdown(const std::vector<double> &returns);

public:
    RiskAdjustedMetrics() : total_return(0.0), total_risk(0.0), sharpe_ratio(0.0),
                            sortino_ratio(0.0), calmar_ratio(0.0),
                            information_ratio(0.0), treynor_ratio(0.0) {}

    double calculate_standard_deviation(const std::vector<double> &returns);
    double calculate_sharpe_ratio(double risk_free_rate);
    double calculate_sortino_ratio(const std::vector<double> &returns, double target_return);
    double calculate_calmar_ratio(const std::vector<double> &returns);
    double calculate_information_ratio(double benchmark_return);
    double calculate_treynor_ratio(double risk_free_rate, double portfolio_beta);

    void set_total_return(double return_value);
    void set_total_risk(double risk_value);
};

#endif