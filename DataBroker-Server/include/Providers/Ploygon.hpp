#ifndef PLOYGON_HPP
#define PLOYGON_HPP

#include "Fetcher/DataFetcher.hpp"

class Ploygon : public DataFetcher
{
public:
    /**
     * @brief Constructor for Ploygon.
     * @param apikey_ API key for authentication.
     * @param secretKey_ Secret key for authentication.
     * @param port_ Port number for connection.
     * @param provider_name_ Name of the provider.
     */
    Ploygon(std::string apikey, std::string secret_key, int port, std::string provider_name)
        : apiKey(apikey), secretKey(secret_key), port(port), provider_name(provider_name) {}

    /**
     * @brief Initialize the Ploygon provider.
     */
    void Initialize();

    /**
     * Gets historical bars.
     * @param symbols The symbols to get bars for.
     * @param timeframe The timeframe for the bars.
     * @param start The start time for the bars.
     * @param end The end time for the bars.
     * @return A vector of Bar objects.
     */
    std::vector<Bar> Get_Historical_Bars(const std::string &symbols, const std::string &timeframe, const std::string &start, const std::string &end);

    /**
     * Gets historical quotes.
     * @param symbols The symbols to get quotes for.
     * @param start The start time for the quotes.
     * @param end The end time for the quotes.
     * @return A vector of Quote objects.
     */
    std::vector<Quote> Get_Historical_Quotes(const std::string &symbols, const std::string &start, const std::string &end);

    /**
     * Gets historical trades.
     * @param symbols The symbols to get trades for.
     * @param start The start time for the trades.
     * @param end The end time for the trades.
     * @return A vector of Trade objects.
     */
    std::vector<Trade> Get_Historical_Trades(const std::string &symbols, const std::string &start, const std::string &end);

    /**
     * Gets the latest bar.
     * @param symbol The symbol to get the latest bar for.
     * @return A Bar object.
     */
    Bar Get_Latest_Bars(const std::string &symbol);

    /**
     * Gets the latest quote.
     * @param symbols The symbols to get the latest quote for.
     * @return A Quote object.
     */
    Quote Get_Latest_Quotes(const std::string &symbols);

    /**
     * Gets the latest trade.
     * @param symbols The symbols to get the latest trade for.
     * @return A Trade object.
     */
    Trade Get_Latest_Trades(const std::string &symbols);

    /**
     * Publishes data.
     * @param data The data to publish.
     * @param info Additional info about the data.
     */
    void Publish(const std::string &data, const std::string &info);

    /**
     * Gets the provider name.
     * @return The provider name.
     */
    std::string Get_Provider_Name()
    {
        return provider_name;
    }

    /**
     * Sets the provider name.
     * @param provider_name The new provider name.
     */
    void Set_Provider_name(std::string provider_name)
    {
        provider_name = provider_name;
    }

protected:
    /**
     * Makes an API request.
     * @param url The URL to make the request to.
     * @return The response from the API.
     */
    std::string MakeApiRequest(const std::string &url);

    /**
     * URL encodes a value.
     * @param value The value to encode.
     * @return The URL encoded value.
     */
    std::string UrlEncode(const std::string &value);

    /**
     * Formats a date string to RFC3339.
     * @param dateStr The date string to format.
     * @return The formatted date string.
     */
    std::string FormatRFC3339(const std::string &dateStr);

    /**
     * Callback for writing data.
     * @param contents The data to write.
     * @param size The size of each element.
     * @param nmemb The number of elements.
     * @param userp The user data.
     * @return The number of bytes written.
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

#endif //PLOYGON_HPP
