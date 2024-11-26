#include "Providers/Alapca.hpp"
#include "Providers/Ploygon.hpp"
#include "Providers/TwelveData.hpp"
#include "Fetcher/DataFetcher.hpp"
#include <thread>
#include "WebSockets/Sockets.hpp"
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>

void Asychronous_publisher(DataFetcher &provider, const std::string &symbols, const std::string &timeframe, const std::string &start, const std::string &end)
{

    std::vector<Bar> bars = provider.GetHistoricalBars(symbols, timeframe, start, end);
    std::string data = "";
    for (auto x : bars)
    {
        data += " open-" + std::to_string(x.open) + " ";
    }
    std::string info = "Author - " + provider.GetProvider_name() + " About - " + symbols + " " + timeframe + " " + start + " " + end;
    provider.publish(data, info);
    return;
}

void runWebSocket(const std::string &provider_name, const char *host, const char *port, const char *text, const std::string &stock)
{
    net::io_context ioc;
    ssl::context ctx{ssl::context::tlsv12_client};
    ctx.set_options(
        ssl::context::default_workarounds |
        ssl::context::no_sslv2 |
        ssl::context::no_sslv3);

    std::shared_ptr<WebSocket> ws = std::make_shared<WebSocket>(ioc, ctx, provider_name);
    ws->run(host, port, text, stock, provider_name);
    ioc.run();
}

int main()
{

    Alapca provider1(std ::getenv("APCA_API_KEY_ID"), std ::getenv("APCA_API_SECRET_KEY"), 5567, "Alpaca");
    provider1.initialize();

    Ploygon provider2(std ::getenv("APCA_API_KEY_ID"), std ::getenv("APCA_API_SECRET_KEY"), 5568, "Ploygon");
    provider2.initialize();

    TwelveData provider3(std ::getenv("APCA_API_KEY_ID"), std ::getenv("APCA_API_SECRET_KEY"), 5569, "TwelveData");
    provider3.initialize();

    std::thread thread1(Asychronous_publisher, std::ref(provider1), "AAPL", "1Day", "2023-01-01", "2023-01-31");
    std::thread thread2(Asychronous_publisher, std::ref(provider2), "AAPL", "1Day", "2023-01-01", "2023-01-31");
    std::thread thread3(Asychronous_publisher, std::ref(provider3), "AAPL", "1Day", "2023-01-01", "2023-01-31");

    thread1.detach();
    thread2.detach();
    thread3.detach();

    // std::thread thread1(runWebSocket, "Alpaca", "stream.data.alpaca.markets", "443", "", "AAPL");
    // std::thread thread2(runWebSocket, "Polygon", "stream.data.alpaca.markets", "443", "", "AAPL");
    // std::thread thread3(runWebSocket, "TwelveData", "stream.data.alpaca.markets", "443", "", "AAPL");

    // thread1.detach();
    // thread2.detach();
    // thread3.detach();

    // Keep the main thread alive to allow detached threads to run
    std::this_thread::sleep_for(std::chrono::hours(1));

    return 0;
}