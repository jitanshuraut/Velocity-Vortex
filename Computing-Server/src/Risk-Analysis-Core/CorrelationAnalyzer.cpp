#include "Risk-Analysis-Core/CorrelationAnalyzer.hpp"
#include <cmath>
#include <numeric>
#include <iostream>
#include <map>
#include <vector>

void CorrelationAnalyzer::load_price_data(std::map<std::string, std::vector<double>> historicalData)
{

    // stock_symbols = {"AAPL", "MSFT", "GOOGL"};
    // price_data["AAPL"] = {150.0, 152.0, 153.0, 155.0, 154.0};
    // price_data["MSFT"] = {250.0, 251.0, 253.0, 252.0, 251.0};
    // price_data["GOOGL"] = {2800.0, 2825.0, 2830.0, 2805.0, 2810.0};

    for (auto x : historicalData)
    {
        price_data[x.first] = x.second;
    }
}

double CorrelationAnalyzer::calculate_correlation(const std::string &stock1, const std::string &stock2)
{
    if (price_data.find(stock1) == price_data.end() || price_data.find(stock2) == price_data.end())
    {
        std::cerr << "Stock symbol not found." << std::endl;
        return 0.0;
    }

    const std::vector<double> &prices1 = price_data[stock1];
    const std::vector<double> &prices2 = price_data[stock2];

    if (prices1.size() != prices2.size())
    {
        std::cerr << "Price data lengths do not match." << std::endl;
        return 0.0;
    }

    double mean1 = std::accumulate(prices1.begin(), prices1.end(), 0.0) / prices1.size();
    double mean2 = std::accumulate(prices2.begin(), prices2.end(), 0.0) / prices2.size();

    double covariance = 0.0;
    double variance1 = 0.0;
    double variance2 = 0.0;

    for (size_t i = 0; i < prices1.size(); ++i)
    {
        covariance += (prices1[i] - mean1) * (prices2[i] - mean2);
        variance1 += (prices1[i] - mean1) * (prices1[i] - mean1);
        variance2 += (prices2[i] - mean2) * (prices2[i] - mean2);
    }

    covariance /= prices1.size();
    variance1 /= prices1.size();
    variance2 /= prices2.size();

    return covariance / (std::sqrt(variance1) * std::sqrt(variance2));
}

std::vector<std::vector<double>> CorrelationAnalyzer::get_correlation_matrix()
{
    size_t n = stock_symbols.size();
    std::vector<std::vector<double>> correlation_matrix(n, std::vector<double>(n, 0.0));

    for (size_t i = 0; i < n; ++i)
    {
        for (size_t j = 0; j < n; ++j)
        {
            correlation_matrix[i][j] = calculate_correlation(stock_symbols[i], stock_symbols[j]);
        }
    }

    return correlation_matrix;
}