/*
1. Fetch Real-Time Stock Quotes
2. Maintain Order Book
3. Execute Trading
*/

#include "Velocity-Bot/bot.hpp"
#include "AlgoEngine-Core/algorithm/SMA.hpp"
#include "AlgoEngine-Core/algorithm/EMA.hpp"
#include "Order-Manager-Core/order_manager.hpp"
#include "IO-Broker-Core/RedisDBManager.hpp"
#include "Data-Fetcher-Core/WebSocket.hpp"
#include "Utilities/Bar.hpp"
#include "Utilities/OHLCV.hpp"
#include "Utilities/Utilities.hpp"
#include "Orderbook/order_book.hpp"
#include "Orderbook/order_.hpp"
#include "Orderbook/transactions.hpp"
#include <thread>

class TestBOT : public Bot, public WebSocket
{
public:
    TestBOT(net::io_context &ioc, ssl::context &ctx)
        : WebSocket(ioc, ctx),
          isRunning(false),
          channel("AAPL"),
          dbFileName("AAPL_Quotes.db"),
          positionOpen(false) {}

    std::atomic<bool> isRunning;
    OrderManager orderManager;
    RedisDBManager publisher;
    Orderbook OrderbookAAPL;
    std::vector<OHLCV> historicalData;
    std::vector<OHLCV> oneMinCandles;
    std::string channel;
    std::string dbFileName;
    bool positionOpen;
    std::atomic<OrderId> CounterId{0};

    void initialize() override
    {
        publisher.connect();
        std::string sqlStructure = "CREATE TABLE IF NOT EXISTS my_table ("
                                   "time TEXT, "
                                   "symbol TEXT, "
                                   "ask_price REAL, "
                                   "ask_quantity REAL, "
                                   "bid_price REAL, "
                                   "bid_quantity REAL);";
        std::string createMessage = "CREATE " + dbFileName + " \"" + sqlStructure + "\"";
        publisher.publishCreateMessage(dbFileName, sqlStructure, {channel});

        WebSocket::run("stream.data.alpaca.markets", "443", "", "AAPL");
    }

public:
    void processMessage(const Json::Value &message) override
    {
        if (!message.isObject())
        {
            std::cerr << "Unexpected message format: not an object." << std::endl;
            return;
        }

        try
        {
            if (message.isMember("T"))
            {
                std::string msgType = message["T"].asString();

                if (msgType == "t")
                {
                    double price = message["p"].asDouble();
                    int size = message["s"].asInt();
                    std::string symbol = message["S"].asString();
                    std::cout << "Trade: " << symbol
                              << " Price: " << price
                              << " Size: " << size << std::endl;
                }
                else if (msgType == "q")
                {
                    double bid = message["bp"].asDouble();
                    double ask = message["ap"].asDouble();
                    std::string symbol = message["S"].asString();

                    std::cout << "Quote: " << symbol
                              << " Bid: " << bid
                              << " Ask: " << ask << std::endl;

                    Price ask_price = static_cast<Price>(message["ap"].asDouble());
                    Quantity ask_quantity = static_cast<Quantity>(message["as"].asDouble());
                    Price bid_price = static_cast<Price>(message["bp"].asDouble());
                    Quantity bid_quantity = static_cast<Quantity>(message["bs"].asDouble());

                    if (ask_quantity > 0)
                    {
                        OrderId orderId_ask = CounterId.fetch_add(1);
                        auto ask_order = std::make_shared<Order_>(
                            OrderType::GoodTillCancel,
                            orderId_ask,
                            Side::Buy,
                            ask_price,
                            ask_quantity);
                        OrderbookAAPL.AddOrder(ask_order);
                        std::cout << "Buy Added" << std::endl;
                    }

                    if (bid_quantity > 0)
                    {
                        OrderId orderId_bid = CounterId.fetch_add(1);
                        auto bid_order = std::make_shared<Order_>(
                            OrderType::GoodTillCancel,
                            orderId_bid,
                            Side::Sell,
                            bid_price,
                            bid_quantity);
                        OrderbookAAPL.AddOrder(bid_order);
                        std::cout << "Sell Added" << std::endl;
                    }
                    OrderbookAAPL.setSpreadThreshold(0.1);
                    auto resp = OrderbookAAPL.executeMarketMakingStrategy(CounterId, 230, 232, 2, 2);
                    std::cout << resp.first << std::endl;
                    // if (resp.first == "buy")
                    // {

                    //     orderManager.createOrder("buy", "market", "ioc", "AAPL", "1", std::nullopt, std::nullopt, std::nullopt, std::nullopt);
                    // }
                    // else
                    // {
                    //     orderManager.createOrder("sell", "market", "ioc", "AAPL", "1", std::nullopt, std::nullopt, std::nullopt, std::nullopt);
                    // }
                }
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error processing message: " << e.what() << std::endl;
        }
    }

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