#ifndef ALAPCA_HPP
#define ALAPCA_HPP

#include "Fetcher/DataFetcher.hpp"

class Alapca : public DataFetcher
{
public:
    Alapca(std::string apikey_, std::string secretKey_, int port_, std::string provider_name_) : apiKey(apikey_), secretKey(secretKey_), port(port_), provider_name(provider_name_) {};
    void initialize();
    std::vector<Bar> GetHistoricalBars(const std::string &symbols, const std::string &timeframe, const std::string &start, const std::string &end);
    std::vector<Quote> GetHistoricalQuotes(const std::string &symbols, const std::string &start, const std::string &end);
    std::vector<Trade> GetHistoricalTrades(const std::string &symbols, const std::string &start, const std::string &end);
    Bar GetLatestBars(const std::string &symbol);
    Quote GetLatestQuotes(const std::string &symbols);
    Trade GetLatestTrades(const std::string &symbols);
    void publish(const std::string &data, const std::string &info);

    std::string GetProvider_name()
    {
        return provider_name;
    }

    std::string SetProvider_name(std::string provider_name_)
    {
        provider_name = provider_name_;
    }

protected:
    std::string makeApiRequest(const std::string &url);
    std::string urlEncode(const std::string &value);
    std::string formatRFC3339(const std::string &dateStr);
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp);

private:
    std::string apiKey;
    std::string secretKey;
    std::string provider_name;
    int port;
    zmq::context_t context{1};
    zmq::socket_t publisher{context, zmq::socket_type::pub};
};

#endif
