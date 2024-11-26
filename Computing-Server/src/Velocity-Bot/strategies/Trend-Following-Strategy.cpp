/*

 1. Initialization: Set up indicators and load historical market data.

 2. Indicator Calculation: Continuously calculate the indicator values using the latest market data.

 3. Decision Making: Analyze indicator values to determine whether to buy, sell, or hold.

 4. Order Execution: Place buy or sell orders based on the decision made.

 5. Database Update: Log trades and relevant metrics into the database for performance tracking.

 6. Stop/Graceful Shutdown: Ensure all orders are handled before stopping the bot.

*/

#include "Velocity-Bot/bot.hpp"
#include "AlgoEngine-Core/algorithm/SMA.hpp"
#include "AlgoEngine-Core/algorithm/EMA.hpp"
#include "AlgoEngine-Core/algorithm/ADX.hpp"
#include "Order-Manager-Core/order_manager.hpp"
#include "IO-Broker-Core/RedisDBManager.hpp"
#include "Data-Fetcher-Core/DataFetcher.hpp"
#include "Utilities/Bar.hpp"
#include "Utilities/OHLCV.hpp"
#include "Utilities/Utilities.hpp"
#include <hiredis/hiredis.h>

#include <thread>
#include <chrono>
#include <atomic>

class BOT_Trend_Following  : public Bot
{
public:
    BOT_Trend_Following() : isRunning(false),
                period_SMA(200),
                period_EMA(50),
                period_ADX(14),
                sma("Simple Moving Average", period_SMA),
                ema("Exponential Moving Average", period_EMA),
                adx("Average Directional Index", period_ADX),
                currentPosition(""),
                channel("AAPL"),
                dbFileName("AAPL_15MIN.db"),
                positionOpen(false) {}

    std::atomic<bool> isRunning;
    int period_SMA;
    int period_EMA;
    int period_ADX;
    SMA sma;
    EMA ema;
    ADX adx;
    DataFetcher fetcher;
    OrderManager orderManager;
    RedisDBManager publisher;
    std::vector<OHLCV> historicalData;
    std::vector<OHLCV> oneMinCandles;
    bool positionOpen;
    std::string currentPosition;
    std::string channel;
    std::string dbFileName;

    void initialize() override
    {
        publisher.connect();
        try
        {
            std::string jsonResponseBars = fetcher.getHistoricalBars("AAPL", "15Min", "2024-05-01", "2024-10-25");
            std::vector<Bar> Bars = fetcher.parseHistoricalBars(jsonResponseBars, "AAPL");

            for (const auto &bar : Bars)
            {
                historicalData.push_back(barToOHLCV(bar));
                sma.addData(barToOHLCV(bar));
                ema.addData(barToOHLCV(bar));
                adx.addData(barToOHLCV(bar));
            }

            std::string sqlStructure = "CREATE TABLE IF NOT EXISTS my_table ("
                                       "time TEXT, "
                                       "symbol TEXT, "
                                       "open REAL, "
                                       "high REAL, "
                                       "low REAL, "
                                       "close REAL, "
                                       "volume INTEGER);";
            std::string createMessage = "CREATE " + dbFileName + " \"" + sqlStructure + "\"";
            publisher.publishCreateMessage(dbFileName, sqlStructure, {channel});
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error during initialization: " << e.what() << std::endl;
        }

        std::cout << "Initialize Done" << std::endl;
        std::cout << "----------------------------------" << std::endl;
        std::cout << "SMA Period: " << sma.getPeriod() << std::endl;
        std::cout << "Historical Data Size: " << sma.getDataSize() << std::endl;
        std::cout << "Signal Size: " << sma.getSignalSize() << std::endl;
        std::cout << "----------------------------------" << std::endl;
    }

    void run() override
    {
        std::cout << "Running" << std::endl;
        std::cout << "----------------------------------" << std::endl;

        isRunning = true;
        while (isRunning)
        {
            try
            {
                // auto marketClockInfo = orderManager.getMarketInfo();
                // for(auto x:marketClockInfo)
                // {
                //     std::cout<<x.first<<" : "<<x.second<<std::endl;
                // }

                // if (marketClockInfo["is_open"] == "true")
                // {
                    processTrading();
                // }
                // else
                // {
                //     std::cout << "Market is closed. Next open time: " << marketClockInfo["next_open"] << std::endl;
                //     std::this_thread::sleep_for(std::chrono::seconds(60));
                // }
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error during run loop: " << e.what() << std::endl;
            }
        }
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
    void processTrading()
    {
        std::cout << "Market is open. Proceeding with trading logic..." << std::endl;
        auto latestCandles = fetcher.getLatestBars("AAPL");

        if (!latestCandles.empty())
        {
            Bar latestBar = fetcher.parseLatestBars(latestCandles, "AAPL");
            std::string dataToInsert =
                "('" + latestBar.time + "', '" + latestBar.symbol + "', " +
                std::to_string(latestBar.open) + ", " +
                std::to_string(latestBar.high) + ", " +
                std::to_string(latestBar.low) + ", " +
                std::to_string(latestBar.close) + ", " +
                std::to_string(latestBar.volume) + ")";

            publisher.publishInsertMessage(dbFileName, dataToInsert, {channel});

            OHLCV currentCandle = barToOHLCV(latestBar);
            oneMinCandles.push_back(currentCandle);

            if (oneMinCandles.size() == 1)
            {
                OHLCV fiveMinCandle = buildAggregatedCandle(oneMinCandles);
                oneMinCandles.clear();

                historicalData.push_back(fiveMinCandle);
                sma.addData(fiveMinCandle);
                ema.addData(fiveMinCandle);
                adx.addData(fiveMinCandle);

                executeTradingLogic();
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    void executeTradingLogic()
    {
        SignalResult smaValue = sma.calculateSignal();
        SignalResult emaValue = ema.calculateSignal();
        SignalResult adxValue = adx.calculateSignal();

        double EMA = emaValue.getSingleValue();
        double SMA = smaValue.getSingleValue();
        double ADX = adxValue.getSingleValue();

        std::cout << "----------------------------------" << std::endl;
        std::cout << "EMA: " << EMA << std::endl;
        std::cout << "SMA: " << SMA << std::endl;
        std::cout << "ADX: " << ADX << std::endl;
        std::cout << "----------------------------------" << std::endl;

        if (!positionOpen && EMA > SMA && ADX > 25)
        {
            orderManager.createOrder("buy", "market", "ioc", "AAPL", "1", std::nullopt, std::nullopt, std::nullopt, std::nullopt);
            currentPosition = "Buy";
            positionOpen = true;
            std::cout << "Buy Signal triggered. Strong trend detected with ADX > 25. Order to buy placed successfully." << std::endl;
        }
        else if (positionOpen && EMA < SMA && ADX < 20)
        {
            orderManager.createOrder("sell", "market", "ioc", "AAPL", "1", std::nullopt, std::nullopt, std::nullopt, std::nullopt);
            currentPosition = "Sell";
            positionOpen = false;
            std::cout << "Sell Signal triggered. Weak trend detected with ADX < 20. Order to sell placed successfully." << std::endl;
        }
        else
        {
            std::cout << "No action taken. Current signals: EMA = " << EMA << ", SMA = " << SMA << ", ADX = " << ADX << std::endl;
        }
    }
};
