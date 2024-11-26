#ifndef PORTFOLIOMANAGER_HPP
#define PORTFOLIOMANAGER_HPP

#include "CorrelationAnalyzer.hpp"
#include "RiskAdjustedMetrics.hpp"
#include "VaRCalculator.hpp"
#include <iostream>
#include <map>
#include <vector>

class PortfolioManager
{
private:
    std::map<std::string, std::vector<double>> StockData;

public:
    PortfolioManager() {}

    void Add_StockData(const std::string &symbol, const std::vector<double> &returns)
    {
        StockData[symbol] = returns;
    }

    void Risk_Analysis_All();
    void Risk_Analysis_Specific(const std::string &symbol);
};

#endif