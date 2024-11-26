#include "Velocity-Bot/bot.hpp"
#include "AlgoEngine-Core/algorithm/SMA.hpp"
#include "AlgoEngine-Core/algorithm/EMA.hpp"
#include "Order-Manager-Core/order_manager.hpp"
#include "IO-Broker-Core/RedisDBManager.hpp"
#include "Utilities/Bar.hpp"
#include "Data-Fetcher-Core/DataFetcher.hpp"
#include "Utilities/OHLCV.hpp"
#include "Utilities/Utilities.hpp"
#include "Orderbook/order_book.hpp"
#include "Orderbook/order_.hpp"
#include "Orderbook/transactions.hpp"
#include <thread>

class TestBOT : public Bot
{
public:
    TestBOT()
        : isRunning(false) {}

    std::atomic<bool> isRunning;
    DataFetcher fetcher;
    OrderManager orderManager;
    RedisDBManager publisher;
    Orderbook OrderbookAAPL;
    std::vector<OHLCV> historicalData;
    std::vector<OHLCV> oneMinCandles;
    bool positionOpen;
    std::string currentPosition;
    std::atomic<OrderId> CounterId{0};

    void initialize() override
    {
        publisher.connect();
        std::string jsonResponseQuotes = fetcher.getHistoricalQuotes("AAPL", "2023-01-01", "2024-01-10");
        std::vector<Quote> quotes = fetcher.parseHistoricalQuotes(jsonResponseQuotes, "AAPL");

        for (const auto &quote : quotes)
        {
            quote.print();
            Price ask_price = static_cast<Price>(quote.askPrice);
            Quantity ask_quantity = static_cast<Quantity>(quote.askSize);

            Price bid_price = static_cast<Price>(quote.bidPrice);
            Quantity bid_quantity = static_cast<Quantity>(quote.bidSize);
            std::cout << "Adding Buy Order with ID: " << CounterId << " at Price: " << ask_price << " and Quantity: " << ask_quantity << std::endl;
            std::cout << std::endl;
            OrderId orderId_ask = CounterId.fetch_add(1);
            OrderbookAAPL.AddOrder(std::make_shared<Order_>(OrderType::GoodTillCancel, orderId_ask, Side::Buy, ask_price, ask_quantity));
            OrderId orderId_bid = CounterId.fetch_add(1);
            std::cout << "Adding Sell Order with ID: " << CounterId << " at Price: " << bid_price << " and Quantity: " << bid_quantity << std::endl;
            OrderbookAAPL.AddOrder(std::make_shared<Order_>(OrderType::GoodTillCancel, orderId_bid, Side::Buy, bid_price, bid_quantity));
            std::cout << std::endl;
            std::cout << quotes.size() << std::endl;
        }
    }

public:
    void run() override
    {
        std::cout << "Running" << std::endl;
        std::cout << "----------------------------------" << std::endl;
    }

    void stop() override
    {
        isRunning = false;
        std::cout << "Bot stopping..." << std::endl;
    }

    bool isRunningState() const
    {
        return isRunning.load();
    }

private:
    unsigned int generateOrderId()
    {
        using namespace std::chrono;
        auto now = system_clock::now();
        auto now_ms = time_point_cast<milliseconds>(now);
        auto epoch = now_ms.time_since_epoch();
        auto value = duration_cast<milliseconds>(epoch).count();
        long long int MOD = 1000000007;
        return static_cast<unsigned int>(value % MOD);
    }
};