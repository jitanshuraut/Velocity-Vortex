#ifndef CORRELATIONANALYZER_HPP
#define CORRELATIONANALYZER_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <map>

class CorrelationAnalyzer
{
private:
    std::vector<std::string> stock_symbols;
    std::unordered_map<std::string, std::vector<double>> price_data;

public:
    CorrelationAnalyzer() {};
    void load_price_data(std::map<std::string, std::vector<double>> historicalData);
    double calculate_correlation(const std::string &stock1, const std::string &stock2);
    std::vector<std::vector<double>> get_correlation_matrix();
};

#endif