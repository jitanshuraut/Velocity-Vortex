#include "Providers/Ploygon.hpp"

// Initializes the Ploygon provider.
void Ploygon::Initialize()
{
    std::cout << "Ploygon initialized" << std::endl;
    publisher.bind("tcp://*:" + std::to_string(port));
}

// Callback function for writing data received from CURL.
// Input: contents - pointer to the data, size - size of each data element, nmemb - number of data elements, userp - user data.
// Output: total size of the data written.
size_t Ploygon::WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp)
{
    size_t totalSize = size * nmemb;
    userp->append((char *)contents, totalSize);
    return totalSize;
}

// URL encodes the given string.
// Input: value - the string to be encoded.
// Output: URL encoded string.
std::string Ploygon::UrlEncode(const std::string &value)
{
    std::ostringstream encoded;
    for (char c : value)
    {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
        {
            encoded << c;
        }
        else
        {
            encoded << '%' << std::setw(2) << std::setfill('0') << std::hex << (int)c;
        }
    }
    return encoded.str();
}

// Formats the given date string to RFC3339 format.
// Input: dateStr - the date string to be formatted.
// Output: RFC3339 formatted date string.
std::string Ploygon::FormatRFC3339(const std::string &dateStr)
{
    if (dateStr.find('T') != std::string::npos)
    {
        return dateStr;
    }
    else
    {
        return dateStr + "T00:00:00Z";
    }
}

// Makes an API request to the given URL.
// Input: url - the URL to make the request to.
// Output: response from the API as a string.
std::string Ploygon::MakeApiRequest(const std::string &url)
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl)
    {
        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "accept: application/json");
        headers = curl_slist_append(headers, ("APCA-API-KEY-ID: " + apiKey).c_str());
        headers = curl_slist_append(headers, ("APCA-API-SECRET-KEY: " + secretKey).c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    return readBuffer;
}

// Retrieves historical bars for the given symbols and timeframe.
// Input: symbols - the symbols to retrieve bars for, timeframe - the timeframe for the bars, start - start date, end - end date.
// Output: vector of Bar objects.
std::vector<Bar> Ploygon::Get_Historical_Bars(const std::string &symbols, const std::string &timeframe, const std::string &start, const std::string &end)
{

    std::ostringstream url;
    url << "https://data.alpaca.markets/v2/stocks/bars?symbols=" << urlEncode(symbols)
        << "&timeframe=" << urlEncode(timeframe)
        << "&start=" << urlEncode(formatRFC3339(start))
        << "&end=" << urlEncode(formatRFC3339(end))
        << "&limit=1000&adjustment=raw&feed=iex&sort=asc";

    Json::Value jsonData;
    Json::Reader jsonReader;
    std::vector<Bar> bars;

    std::string jsonResponse = makeApiRequest(url.str());

    if (jsonReader.parse(jsonResponse, jsonData))
    {

        const Json::Value jsonBars = jsonData["bars"][symbols];

        for (const auto &bar : jsonBars)
        {
            Bar b(symbols);
            b.fromJson(bar);
            bars.push_back(b);
        }
    }
    else
    {
        std::cerr << "Error parsing historical bars JSON response: " << jsonReader.getFormattedErrorMessages() << std::endl;
    }
    return bars;
}

// Retrieves the latest bar for the given symbols.
// Input: symbols - the symbols to retrieve the latest bar for.
// Output: Bar object.
Bar Ploygon::Get_Latest_Bars(const std::string &symbols)
{
    std::ostringstream url;
    url << "https://data.alpaca.markets/v2/stocks/bars/latest?symbols=" << urlEncode(symbols);

    Json::Value jsonData;
    Json::Reader jsonReader;
    Bar bar(symbols);
    std::string jsonResponse = makeApiRequest(url.str());
    if (jsonReader.parse(jsonResponse, jsonData))
    {

        const Json::Value &barData = jsonData["bars"][symbols];
        if (!barData.isNull())
        {
            bar.fromJson(barData);
            bar.print();
        }
        else
        {
            std::cerr << "Error: No data found for symbol " << symbols << std::endl;
        }
    }
    else
    {
        std::cerr << "Error parsing latest bars JSON response: " << jsonReader.getFormattedErrorMessages() << std::endl;
    }
    bar.print();
    return bar;
}

// Retrieves historical quotes for the given symbols and date range.
// Input: symbols - the symbols to retrieve quotes for, start - start date, end - end date.
// Output: vector of Quote objects.
std::vector<Quote> Ploygon::Get_Historical_Quotes(const std::string &symbols, const std::string &start, const std::string &end)
{
    std::ostringstream url;
    url << "https://data.alpaca.markets/v2/stocks/quotes?symbols=" << urlEncode(symbols)
        << "&start=" << urlEncode(formatRFC3339(start))
        << "&end=" << urlEncode(formatRFC3339(end))
        << "&limit=10000&sort=asc";

    Json::Value jsonData;
    Json::Reader jsonReader;
    std::vector<Quote> quotes;
    std::string jsonResponse = makeApiRequest(url.str());

    if (jsonReader.parse(jsonResponse, jsonData))
    {

        const Json::Value jsonQuotes = jsonData["quotes"][symbols];
        for (const auto &quote : jsonQuotes)
        {
            Quote q(symbols);
            q.fromJson(quote);
            quotes.push_back(q);
        }
    }
    else
    {
        std::cerr << "Error parsing historical quotes JSON response: " << jsonReader.getFormattedErrorMessages() << std::endl;
    }
    return quotes;
}

// Retrieves the latest quote for the given symbols.
// Input: symbols - the symbols to retrieve the latest quote for.
// Output: Quote object.
Quote Ploygon::Get_Latest_Quotes(const std::string &symbols)
{
    std::ostringstream url;
    url << "https://data.alpaca.markets/v2/stocks/quotes/latest?symbols=" << urlEncode(symbols);

    std::string jsonResponse = makeApiRequest(url.str());
    Json::Value jsonData;
    Json::Reader jsonReader;
    Quote quote(symbols);

    if (jsonReader.parse(jsonResponse, jsonData))
    {
        quote.fromJson(jsonData["quote"]);
    }
    else
    {
        std::cerr << "Error parsing latest quotes JSON response: " << jsonReader.getFormattedErrorMessages() << std::endl;
    }
    return quote;
}

// Retrieves historical trades for the given symbols and date range.
// Input: symbols - the symbols to retrieve trades for, start - start date, end - end date.
// Output: vector of Trade objects.
std::vector<Trade> Ploygon::Get_Historical_Trades(const std::string &symbols, const std::string &start, const std::string &end)
{
    std::ostringstream url;
    url << "https://data.alpaca.markets/v2/stocks/trades?symbols=" << urlEncode(symbols)
        << "&start=" << urlEncode(formatRFC3339(start))
        << "&end=" << urlEncode(formatRFC3339(end))
        << "&limit=1000&sort=asc";
    std::cout << url.str() << std::endl;

    std::string jsonResponse = makeApiRequest(url.str());
    Json::Value jsonData;
    Json::Reader jsonReader;
    std::vector<Trade> trades;

    if (jsonReader.parse(jsonResponse, jsonData))
    {
        const Json::Value jsonTrades = jsonData["trades"][symbols];
        for (const auto &trade : jsonTrades)
        {
            Trade t(symbols);
            t.fromJson(trade);
            trades.push_back(t);
        }
    }
    else
    {
        std::cerr << "Error parsing historical trades JSON response: " << jsonReader.getFormattedErrorMessages() << std::endl;
    }
    return trades;
}

// Retrieves the latest trade for the given symbols.
// Input: symbols - the symbols to retrieve the latest trade for.
// Output: Trade object.
Trade Ploygon::Get_Latest_Trades(const std::string &symbols)
{
    std::ostringstream url;
    url << "https://data.alpaca.markets/v2/stocks/trades/latest?symbols=" << urlEncode(symbols);

    Json::Value jsonData;
    Json::Reader jsonReader;
    Trade trade(symbols);

    std::string jsonResponse = makeApiRequest(url.str());

    if (jsonReader.parse(jsonResponse, jsonData))
    {
        trade.fromJson(jsonData["trade"]);
    }
    else
    {
        std::cerr << "Error parsing latest trades JSON response: " << jsonReader.getFormattedErrorMessages() << std::endl;
    }
    return trade;
}

// Publishes the given data with the provided info.
// Input: data - the data to be published, info - additional info to be included.
void Ploygon::Publish(const std::string &data, const std::string &info)
{
    std::string bundle = info + ":" + data;
    zmq::message_t message(bundle.size());
    std::memcpy(message.data(), bundle.data(), bundle.size());
    publisher.send(message, zmq::send_flags::none);
    std::cout << "Published: " << info << std::endl;
}