#include "Data-Fetcher-Core/DataFetcher.hpp"
#include <curl/curl.h>
#include <iostream>
#include <cstdlib>
#include <json/json.h>
#include <iomanip>
#include <sstream>

DataFetcher::DataFetcher() {
    apiKey = std::getenv("APCA_API_KEY_ID");
    secretKey = std::getenv("APCA_API_SECRET_KEY");
}

std::string DataFetcher::urlEncode(const std::string& value) {
    std::ostringstream encoded;
    for (char c : value) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded << c;
        } else {
            encoded << '%' << std::setw(2) << std::setfill('0') << std::hex << (int)c;
        }
    }
    return encoded.str();
}

std::string DataFetcher::formatRFC3339(const std::string& dateStr) {
    if (dateStr.find('T') != std::string::npos) {
        return dateStr;
    } else {
        return dateStr + "T00:00:00Z";
    }
}

size_t DataFetcher::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

std::string DataFetcher::makeApiRequest(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "accept: application/json");
        headers = curl_slist_append(headers, ("APCA-API-KEY-ID: " + apiKey).c_str());
        headers = curl_slist_append(headers, ("APCA-API-SECRET-KEY: " + secretKey).c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    return readBuffer;
}

std::vector<Bar> DataFetcher::parseHistoricalBars(const std::string& jsonResponse, std::string sys) {
    Json::Value jsonData;
    Json::Reader jsonReader;
    std::vector<Bar> bars;

    std::cout << "JSON Response: " << jsonResponse << std::endl;

    if (jsonReader.parse(jsonResponse, jsonData)) {
       
            const Json::Value jsonBars = jsonData["bars"][sys];
          
                for (const auto& bar : jsonBars) {
                    Bar b(sys);
                    b.fromJson(bar);
                    bars.push_back(b);
                }
            
    } else {
        std::cerr << "Error parsing historical bars JSON response: " << jsonReader.getFormattedErrorMessages() << std::endl;
    }
    return bars;
}

Bar DataFetcher::parseLatestBars(const std::string& jsonResponse,std::string sys) {
       Json::Value jsonData;
    Json::Reader jsonReader;
    Bar bar(sys); 


    if (jsonReader.parse(jsonResponse, jsonData)) {

        const Json::Value& barData = jsonData["bars"][sys];
        if (!barData.isNull()) {
            bar.fromJson(barData);
            bar.print(); 
        } else {
            std::cerr << "Error: No data found for symbol " << sys << std::endl;
        }
    } else {
        std::cerr << "Error parsing latest bars JSON response: " << jsonReader.getFormattedErrorMessages() << std::endl;
    }
    bar.print();
    return bar; 
}

std::vector<Quote> DataFetcher::parseHistoricalQuotes(const std::string& jsonResponse, std::string sys) {
    Json::Value jsonData;
    Json::Reader jsonReader;
    std::vector<Quote> quotes;

    if (jsonReader.parse(jsonResponse, jsonData)) {

            const Json::Value jsonQuotes = jsonData["quotes"][sys];
            for (const auto& quote : jsonQuotes) {
                Quote q(sys);
                q.fromJson(quote);
                quotes.push_back(q);
            }
        
    } else {
        std::cerr << "Error parsing historical quotes JSON response: " << jsonReader.getFormattedErrorMessages() << std::endl;
    }
    return quotes;
}

Quote DataFetcher::parseLatestQuotes(const std::string& jsonResponse,std::string sys) {
    Json::Value jsonData;
    Json::Reader jsonReader;
    Quote quote(sys);

    if (jsonReader.parse(jsonResponse, jsonData)) {
        quote.fromJson(jsonData["quote"]);
    } else {
        std::cerr << "Error parsing latest quotes JSON response: " << jsonReader.getFormattedErrorMessages() << std::endl;
    }
    return quote;
}

std::vector<Trade> DataFetcher::parseHistoricalTrades(const std::string& jsonResponse, std::string sys) {
    Json::Value jsonData;
    Json::Reader jsonReader;
    std::vector<Trade> trades;

    if (jsonReader.parse(jsonResponse, jsonData)) {
        const Json::Value jsonTrades = jsonData["trades"][sys];
        for (const auto& trade : jsonTrades) {
            Trade t(sys);
            t.fromJson(trade);
            trades.push_back(t);
        }
    } else {
        std::cerr << "Error parsing historical trades JSON response: " << jsonReader.getFormattedErrorMessages() << std::endl;
    }
    return trades;
}

Trade DataFetcher::parseLatestTrades(const std::string& jsonResponse,std::string sys) {
    Json::Value jsonData;
    Json::Reader jsonReader;
    Trade trade(sys);

    if (jsonReader.parse(jsonResponse, jsonData)) {
        trade.fromJson(jsonData["trade"]);
    } else {
        std::cerr << "Error parsing latest trades JSON response: " << jsonReader.getFormattedErrorMessages() << std::endl;
    }
    return trade;
}

std::string DataFetcher::getHistoricalBars(const std::string& symbols, const std::string& timeframe,
                                           const std::string& start, const std::string& end) {
    std::ostringstream url;
    url << "https://data.alpaca.markets/v2/stocks/bars?symbols=" << urlEncode(symbols)
        << "&timeframe=" << urlEncode(timeframe)
        << "&start=" << urlEncode(formatRFC3339(start))
        << "&end=" << urlEncode(formatRFC3339(end))
        << "&limit=1000&adjustment=raw&feed=iex&sort=asc";
    return makeApiRequest(url.str());
}

std::string DataFetcher::getLatestBars(const std::string& symbols) {
    std::ostringstream url;
    url << "https://data.alpaca.markets/v2/stocks/bars/latest?symbols=" << urlEncode(symbols);
    return makeApiRequest(url.str());
}

std::string DataFetcher::getHistoricalQuotes(const std::string& symbols, const std::string& start, const std::string& end) {
    std::ostringstream url;
    url << "https://data.alpaca.markets/v2/stocks/quotes?symbols=" << urlEncode(symbols)
        << "&start=" << urlEncode(formatRFC3339(start))
        << "&end=" << urlEncode(formatRFC3339(end))
        << "&limit=10000&sort=asc";
    return makeApiRequest(url.str());
}

std::string DataFetcher::getLatestQuotes(const std::string& symbols) {
    std::ostringstream url;
    url << "https://data.alpaca.markets/v2/stocks/quotes/latest?symbols=" << urlEncode(symbols);
    return makeApiRequest(url.str());
}

std::string DataFetcher::getHistoricalTrades(const std::string& symbols, const std::string& start, const std::string& end) {
    std::ostringstream url;
    url << "https://data.alpaca.markets/v2/stocks/trades?symbols=" << urlEncode(symbols)
        << "&start=" << urlEncode(formatRFC3339(start))
        << "&end=" << urlEncode(formatRFC3339(end))
        << "&limit=1000&sort=asc";
    std::cout << url.str() << std::endl;
    return makeApiRequest(url.str());
}

std::string DataFetcher::getLatestTrades(const std::string& symbols) {
    std::ostringstream url;
    url << "https://data.alpaca.markets/v2/stocks/trades/latest?symbols=" << urlEncode(symbols);
    return makeApiRequest(url.str());
}