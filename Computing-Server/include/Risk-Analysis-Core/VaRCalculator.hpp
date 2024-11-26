#ifndef VARCALCULATOR_HPP
#define VARCALCULATOR_HPP

#include <vector>

class VaRCalculator
{
private:
    std::vector<double> historical_returns;
    double confidence_level;
    double var_value;

public:
    VaRCalculator(double confidence_level, double var_val) : confidence_level(confidence_level), var_value(var_val) {};
    void calculate_var();
    double get_var();
    void set_historical_returns(const std::vector<double> &returns);
};

#endif