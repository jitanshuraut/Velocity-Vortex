#ifndef DATAFETCHER_HPP
#define DATAFETCHER_HPP

#include <string>
#include <sstream>
#include <iomanip>
#include <curl/curl.h>
#include <vector>
#include "Utilities/Bar.hpp"     
#include "Utilities/Quote.hpp"
#include "Utilities/Trade.hpp"   

class DataFetcher {
public:
    DataFetcher();
    

    std::string getHistoricalBars(const std::string& symbols, const std::string& timeframe,
                                   const std::string& start, const std::string& end);
    std::vector<Bar> parseHistoricalBars(const std::string& jsonResponse,std::string sys);


    
    std::string getHistoricalQuotes(const std::string& symbols, const std::string& start, const std::string& end);
    std::vector<Quote> parseHistoricalQuotes(const std::string& jsonResponse,std::string sys);


    
    std::string getHistoricalTrades(const std::string& symbols, const std::string& start, const std::string& end);
    std::vector<Trade> parseHistoricalTrades(const std::string& jsonResponse,std::string sys);

    
    std::string getLatestBars(const std::string& symbol);
    Bar parseLatestBars(const std::string& jsonResponse,std::string sys);
    
    std::string getLatestQuotes(const std::string& symbols);
    Quote parseLatestQuotes(const std::string& jsonResponse,std::string sys);
    
    std::string getLatestTrades(const std::string& symbols);
    Trade parseLatestTrades(const std::string& jsonResponse,std::string sys);

private:
    std::string apiKey;
    std::string secretKey;
    
    std::string urlEncode(const std::string& value);
    std::string formatRFC3339(const std::string& dateStr);
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
    std::string makeApiRequest(const std::string& url);
};

#endif 