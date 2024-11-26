#ifndef TWELVEDATA_HPP
#define TWELVEDATA_HPP

#include "Fetcher/DataFetcher.hpp"

class TwelveData : public DataFetcher
{
public:
    /**
     * @brief Constructor for TwelveData.
     * @param apikey_ API key for authentication.
     * @param secretKey_ Secret key for authentication.
     * @param port_ Port number for the connection.
     * @param provider_name_ Name of the data provider.
     */
    TwelveData(std::string apikey_, std::string secretKey_, int port_, std::string provider_name_)
        : apiKey(apikey_), secretKey(secretKey_), port(port_), provider_name(provider_name_) {}

    /**
     * @brief Initialize the data provider.
     */
    void Initialize();

    /**
     * @brief Get historical bars data.
     * @param symbols Symbols to fetch data for.
     * @param timeframe Timeframe for the data.
     * @param start Start date for the data.
     * @param end End date for the data.
     * @return Vector of Bar objects.
     */
    std::vector<Bar> Get_Historical_Bars(const std::string &symbols, const std::string &timeframe, const std::string &start, const std::string &end);

    /**
     * @brief Get historical quotes data.
     * @param symbols Symbols to fetch data for.
     * @param start Start date for the data.
     * @param end End date for the data.
     * @return Vector of Quote objects.
     */
    std::vector<Quote> Get_Historical_Quotes(const std::string &symbols, const std::string &start, const std::string &end);

    /**
     * @brief Get historical trades data.
     * @param symbols Symbols to fetch data for.
     * @param start Start date for the data.
     * @param end End date for the data.
     * @return Vector of Trade objects.
     */
    std::vector<Trade> Get_Historical_Trades(const std::string &symbols, const std::string &start, const std::string &end);

    /**
     * @brief Get the latest bar data.
     * @param symbol Symbol to fetch data for.
     * @return Latest Bar object.
     */
    Bar Get_Latest_Bars(const std::string &symbol);

    /**
     * @brief Get the latest quotes data.
     * @param symbols Symbols to fetch data for.
     * @return Latest Quote object.
     */
    Quote Get_Latest_Quotes(const std::string &symbols);

    /**
     * @brief Get the latest trades data.
     * @param symbols Symbols to fetch data for.
     * @return Latest Trade object.
     */
    Trade Get_Latest_Trades(const std::string &symbols);

    /**
     * @brief Publish data.
     * @param data Data to be published.
     * @param info Additional information.
     */
    void Publish(const std::string &data, const std::string &info);

    /**
     * @brief Get the provider name.
     * @return Provider name.
     */
    std::string Get_Provider_name() { return provider_name; }

    /**
     * @brief Set the provider name.
     * @param provider_name_ New provider name.
     */
    void Set_Provider_name(std::string provider_name_)
    {
        provider_name = provider_name_;
    }

protected:
    /**
     * @brief Make an API request.
     * @param url URL for the API request.
     * @return Response from the API.
     */
    std::string MakeApiRequest(const std::string &url);

    /**
     * @brief URL encode a value.
     * @param value Value to be encoded.
     * @return Encoded value.
     */
    std::string UrlEncode(const std::string &value);

    /**
     * @brief Format a date string to RFC3339.
     * @param dateStr Date string to be formatted.
     * @return Formatted date string.
     */
    std::string FormatRFC3339(const std::string &dateStr);

    /**
     * @brief Callback function for writing data.
     * @param contents Data contents.
     * @param size Size of each data element.
     * @param nmemb Number of data elements.
     * @param userp User data pointer.
     * @return Number of bytes written.
     */
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp);

private:
    std::string apiKey;
    std::string secretKey;
    std::string provider_name;
    int port;
    zmq::context_t context{1};
    zmq::socket_t publisher{context, zmq::socket_type::pub};
};

#endif // TWELVEDATA_HPP
