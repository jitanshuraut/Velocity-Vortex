#ifndef DATAFETCHER_HPP
#define DATAFETCHER_HPP

#include <string>
#include <vector>
#include "Utilities/Bar.hpp"
#include "Utilities/Quote.hpp"
#include "Utilities/Trade.hpp"
#include <curl/curl.h>

class DataFetcher
{
public:
    DataFetcher() = default;
    virtual ~DataFetcher() = default;

    virtual std::vector<Bar> GetHistoricalBars(const std::string &symbols, const std::string &timeframe, const std::string &start, const std::string &end) = 0;
    virtual std::vector<Quote> GetHistoricalQuotes(const std::string &symbols, const std::string &start, const std::string &end) = 0;
    virtual std::vector<Trade> GetHistoricalTrades(const std::string &symbols, const std::string &start, const std::string &end) = 0;
    virtual Bar GetLatestBars(const std::string &symbol) = 0;
    virtual Quote GetLatestQuotes(const std::string &symbols) = 0;
    virtual Trade GetLatestTrades(const std::string &symbols) = 0;
    virtual void publish(const std::string &data, const std::string &info) = 0;
    virtual std::string GetProvider_name()=0;
    virtual std::string SetProvider_name(std::string provider_name_)=0;

protected:
    virtual std::string makeApiRequest(const std::string &url) = 0;
};

#endif
