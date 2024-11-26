#include "Risk-Analysis-Core/PortfolioManager.hpp"
#include "Risk-Analysis-Core/CorrelationAnalyzer.hpp"
#include "Risk-Analysis-Core/RiskAdjustedMetrics.hpp"
#include "Risk-Analysis-Core/VaRCalculator.hpp"
#include <numeric>
#include <iomanip>
#include <iostream>

void PortfolioManager::Risk_Analysis_All()
{
    RiskAdjustedMetrics riskMetrics;
    VaRCalculator varCalculator(0.95, 0.0);
    CorrelationAnalyzer correlationAnalyzer;
    correlationAnalyzer.load_price_data(StockData);

    std::cout << "Performing risk analysis for all stocks..." << std::endl;

    for (const auto &stock : StockData)
    {
        const std::string &symbol = stock.first;
        const std::vector<double> &returns = stock.second;

        double totalReturn = std::accumulate(returns.begin(), returns.end(), 0.0);
        riskMetrics.set_total_return(totalReturn);
        double totalRisk = riskMetrics.calculate_standard_deviation(returns);
        riskMetrics.set_total_risk(totalRisk);

        double sharpeRatio = riskMetrics.calculate_sharpe_ratio(0.01);
        double sortinoRatio = riskMetrics.calculate_sortino_ratio(returns, 0.01);
        double calmarRatio = riskMetrics.calculate_calmar_ratio(returns);

        varCalculator.set_historical_returns(returns);
        varCalculator.calculate_var();
        double varValue = varCalculator.get_var();

        std::cout << "Stock: " << symbol << std::endl;
        std::cout << "Total Return: " << std::fixed << std::setprecision(2) << totalReturn << std::endl;
        std::cout << "Total Risk (Std Dev): " << totalRisk << std::endl;
        std::cout << "Sharpe Ratio: " << sharpeRatio << std::endl;
        std::cout << "Sortino Ratio: " << sortinoRatio << std::endl;
        std::cout << "Calmar Ratio: " << calmarRatio << std::endl;
        std::cout << "Value at Risk (95%): " << varValue << std::endl;

        for (const auto &other_stock : StockData)
        {
            if (other_stock.first != symbol)
            {
                double correlation = correlationAnalyzer.calculate_correlation(symbol, other_stock.first);
                std::cout << "Correlation with " << other_stock.first << ": " << correlation << std::endl;
            }
        }

        std::cout << "-----------------------------------" << std::endl;
    }
}

void PortfolioManager::Risk_Analysis_Specific(const std::string &symbol)
{
    if (StockData.find(symbol) == StockData.end())
    {
        std::cout << "Stock symbol not found: " << symbol << std::endl;
        return;
    }

    RiskAdjustedMetrics riskMetrics;
    VaRCalculator varCalculator(0.95, 0.0);
    CorrelationAnalyzer correlationAnalyzer;

    correlationAnalyzer.load_price_data(StockData);

    const std::vector<double> &returns = StockData[symbol];

    double totalReturn = std::accumulate(returns.begin(), returns.end(), 0.0);
    riskMetrics.set_total_return(totalReturn);
    double totalRisk = riskMetrics.calculate_standard_deviation(returns);
    riskMetrics.set_total_risk(totalRisk);

    double sharpeRatio = riskMetrics.calculate_sharpe_ratio(0.01);
    double sortinoRatio = riskMetrics.calculate_sortino_ratio(returns, 0.01);
    double calmarRatio = riskMetrics.calculate_calmar_ratio(returns);

    varCalculator.set_historical_returns(returns);
    varCalculator.calculate_var();
    double varValue = varCalculator.get_var();

    std::cout << "Risk Analysis for Stock: " << symbol << std::endl;
    std::cout << "Total Return: " << std::fixed << std::setprecision(2) << totalReturn << std::endl;
    std::cout << "Total Risk (Std Dev): " << totalRisk << std::endl;
    std::cout << "Sharpe Ratio: " << sharpeRatio << std::endl;
    std::cout << "Sortino Ratio: " << sortinoRatio << std::endl;
    std::cout << "Calmar Ratio: " << calmarRatio << std::endl;
    std::cout << "Value at Risk (95%): " << varValue << std::endl;

    for (const auto &other_stock : StockData)
    {
        if (other_stock.first != symbol)
        {
            double correlation = correlationAnalyzer.calculate_correlation(symbol, other_stock.first);
            std::cout << "Correlation with " << other_stock.first << ": " << correlation << std::endl;
        }
    }

    std::cout << "-----------------------------------" << std::endl;
}