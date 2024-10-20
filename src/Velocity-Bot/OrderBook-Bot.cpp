// /*

//  1. Initialization: Set up indicators and load historical market data.

//  2. Indicator Calculation: Continuously calculate the indicator values using the latest market data.

//  3. Decision Making: Analyze indicator values to determine whether to buy, sell, or hold.

//  4. Order Execution: Place buy or sell orders based on the decision made.

//  5. Database Update: Log trades and relevant metrics into the database for performance tracking.

//  6. Stop/Graceful Shutdown: Ensure all orders are handled before stopping the bot.

// */

// #include "Velocity-Bot/bot.hpp"
// #include "AlgoEngine-Core/algorithm/SMA.hpp"
// #include "AlgoEngine-Core/algorithm/EMA.hpp"
// #include "Order-Manager-Core/order_manager.hpp"
// #include "IO-Broker-Core/RedisDBManager.hpp"
// #include "Data-Fetcher-Core/WebSocket.cpp"
// #include "Utilities/Bar.hpp"
// #include "Utilities/OHLCV.hpp"
// #include "Utilities/Utilities.hpp"
// #include "Orderbook/limit_order.hpp"
// #include "Orderbook/order_book.hpp"
// #include "Orderbook/order_orderBook.hpp"
// #include "Orderbook/order_status.hpp"
// #include "Orderbook/order_tree.hpp"
// #include "Orderbook/order_type.hpp"
// #include "Orderbook/transactions.hpp"

// #include <thread>
// #include <chrono>
// #include <atomic>

// class TestBOT : public Bot
// {
// public:
//     TestBOT() : isRunning(false),
//                 period_SMA(12),
//                 period_EMA(12),
//                 sma("Simple Moving Average", period_SMA),
//                 ema("Exponential Moving Average", period_EMA),
//                 currentPosition(""),
//                 positionOpen(false) {}

//     std::atomic<bool> isRunning;
//     int period_SMA;
//     int period_EMA;
//     SMA sma;
//     EMA ema;

//     OrderManager orderManager;
//     RedisDBManager publisher;
//     WebSocket endpoint;
//     OrderBook OrderbookAAPL;
//     std::vector<OHLCV> historicalData;
//     std::vector<OHLCV> oneMinCandles;
//     bool positionOpen;
//     std::string currentPosition;

//     void initialize() override
//     {
//         publisher.connect();

//         std::thread t([this]()
//                       { endpoint.run("wss://stream.data.alpaca.markets/v2/iex"); });

//         std::this_thread::sleep_for(std::chrono::seconds(2));

//         Json::Value auth_msg;
//         auth_msg["action"] = "auth";
//         auth_msg["key"] = std::getenv("APCA_API_KEY_ID");
//         auth_msg["secret"] = std::getenv("APCA_API_SECRET_KEY");
//         Json::FastWriter writer;
//         endpoint.send(writer.write(auth_msg));

//         std::this_thread::sleep_for(std::chrono::seconds(2));

//         Json::Value subscribe_msg;
//         subscribe_msg["action"] = "subscribe";
//         subscribe_msg["trades"].append("AAPL");
//         subscribe_msg["quotes"].append("AMD");
//         subscribe_msg["quotes"].append("CLDR");
//         subscribe_msg["bars"].append("*");
//         subscribe_msg["dailyBars"].append("VOO");
//         subscribe_msg["statuses"].append("*");
//         endpoint.send(writer.write(subscribe_msg));
//         t.detach();
//     }

//     void run() override
//     {
//         std::cout << "Running" << std::endl;
//         std::cout << "----------------------------------" << std::endl;

//         isRunning = true;
//         while (isRunning)
//         {
//             try
//             {
//                 auto marketClockInfo = orderManager.getMarketInfo();

//                 if (marketClockInfo["is_open"] == "true")
//                 {
//                     processTrading();
//                 }
//                 else
//                 {
//                     std::cout << "Market is closed. Next open time: " << marketClockInfo["next_open"] << std::endl;
//                     std::this_thread::sleep_for(std::chrono::seconds(60));
//                 }
//             }
//             catch (const std::exception &e)
//             {
//                 std::cerr << "Error during run loop: " << e.what() << std::endl;
//             }
//         }
//     }

//     void stop() override
//     {
//         isRunning = false;
//         std::cout << "Bot stopping..." << std::endl;
//     }

//     bool isRunningState() const
//     {
//         return isRunning.load();
//     }

// private:
//     void processTrading()
//     {
//         std::map<std::string, std::string> messageData = endpoint.on_message(m_hdl);
//         // OrderbookAAPL.processOrder(Order(std::move(order_symbol), std::move(order_id), std::move(order_type), std::move(order_price), std::move(order_qty)));
//     }

//     void executeTradingLogic()
//     {
//         SignalResult smaValue = sma.calculateSignal();
//         SignalResult emaValue = ema.calculateSignal();

//         double shortEMA = smaValue.getSingleValue();
//         double longSMA = emaValue.getSingleValue();

//         std::cout << "----------------------------------" << std::endl;
//         std::cout << "Short EMA: " << shortEMA << std::endl;
//         std::cout << "Long SMA: " << longSMA << std::endl;
//         std::cout << "Position Open: " << positionOpen << std::endl;
//         std::cout << "Current Position: " << currentPosition << std::endl;
//         std::cout << "----------------------------------" << std::endl;

//         if (!positionOpen && shortEMA > longSMA)
//         {
//             orderManager.createOrder("buy", "market", "ioc", "AAPL", "1", std::nullopt, std::nullopt, std::nullopt, std::nullopt);
//             currentPosition = "Buy";
//             positionOpen = true;
//             std::cout << "Buy Signal triggered. Order to buy placed successfully." << std::endl;
//         }
//         else if (positionOpen && shortEMA < longSMA)
//         {
//             orderManager.createOrder("sell", "market", "ioc", "AAPL", "1", std::nullopt, std::nullopt, std::nullopt, std::nullopt);
//             currentPosition = "Sell";
//             positionOpen = false;
//             std::cout << "Sell Signal triggered. Order to sell placed successfully." << std::endl;
//         }
//         else
//         {
//             std::cout << "No action taken. Current signals: Short EMA = " << shortEMA << ", Long SMA = " << longSMA << std::endl;
//         }
//     }
// };
