#ifndef DATAFETCHER_HPP
#define DATAFETCHER_HPP

#include <curl/curl.h>
#include <string>
#include <vector>
#include "Utilities/Bar.hpp"
#include "Utilities/Quote.hpp"
#include "Utilities/Trade.hpp"

class DataFetcher
{
public:
    DataFetcher() = default;
    virtual ~DataFetcher() = default;

    /**
     * @brief Fetches historical bars for the given symbols and timeframe.
     * @param symbols The symbols to fetch bars for.
     * @param timeframe The timeframe for the bars.
     * @param start The start date for the bars.
     * @param end The end date for the bars.
     * @return A vector of Bar objects.
     */
    virtual std::vector<Bar> Get_Historical_Bars(const std::string &symbols, const std::string &timeframe, const std::string &start, const std::string &end) = 0;

    /**
     * @brief Fetches historical quotes for the given symbols.
     * @param symbols The symbols to fetch quotes for.
     * @param start The start date for the quotes.
     * @param end The end date for the quotes.
     * @return A vector of Quote objects.
     */
    virtual std::vector<Quote> Get_Historical_Quotes(const std::string &symbols, const std::string &start, const std::string &end) = 0;

    /**
     * @brief Fetches historical trades for the given symbols.
     * @param symbols The symbols to fetch trades for.
     * @param start The start date for the trades.
     * @param end The end date for the trades.
     * @return A vector of Trade objects.
     */
    virtual std::vector<Trade> Get_Historical_Trades(const std::string &symbols, const std::string &start, const std::string &end) = 0;

    /**
     * @brief Fetches the latest bar for the given symbol.
     * @param symbol The symbol to fetch the latest bar for.
     * @return A Bar object.
     */
    virtual Bar Get_Latest_Bars(const std::string &symbol) = 0;

    /**
     * @brief Fetches the latest quote for the given symbols.
     * @param symbols The symbols to fetch the latest quote for.
     * @return A Quote object.
     */
    virtual Quote Get_Latest_Quotes(const std::string &symbols) = 0;

    /**
     * @brief Fetches the latest trade for the given symbols.
     * @param symbols The symbols to fetch the latest trade for.
     * @return A Trade object.
     */
    virtual Trade Get_Latest_Trades(const std::string &symbols) = 0;

    /**
     * @brief Publishes the given data with additional info.
     * @param data The data to publish.
     * @param info Additional info to publish with the data.
     */
    virtual void Publish(const std::string &data, const std::string &info) = 0;

    /**
     * @brief Gets the provider name.
     * @return The provider name as a string.
     */
    virtual std::string Get_Provider_name() = 0;

    /**
     * @brief Sets the provider name.
     * @param provider_name_ The provider name to set.
     * @return The provider name as a string.
     */
    virtual std::string Set_Provider_name(std::string provider_name_) = 0;

protected:
    /**
     * @brief Makes an API request to the given URL.
     * @param url The URL to make the API request to.
     * @return The response from the API request as a string.
     */
    virtual std::string MakeApiRequest(const std::string &url) = 0;
};

#endif // DATAFETCHER_HPP
