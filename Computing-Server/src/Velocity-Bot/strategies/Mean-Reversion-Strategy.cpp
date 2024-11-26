/*

 1. Initialization: Set up indicators and load historical market data.

 2. Indicator Calculation: Continuously calculate the indicator values using the latest market data.

 3. Decision Making: Analyze indicator values to determine whether to buy, sell, or hold.

 4. Order Execution: Place buy or sell orders based on the decision made.

 5. Database Update: Log trades and relevant metrics into the database for performance tracking.

 6. Stop/Graceful Shutdown: Ensure all orders are handled before stopping the bot.

*/

#include "Velocity-Bot/bot.hpp"
#include "AlgoEngine-Core/algorithm/RSI.hpp"
#include "AlgoEngine-Core/algorithm/BollingerBands.hpp"
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

class Mean_Reversion : public Bot
{
public:
    Mean_Reversion() : isRunning(false),
                       period_BB(20),
                       STD_BB(2),
                       period_RSI(14),
                       rsi("Relative Strength Index.", period_RSI),
                       bb("BollingerBands", period_BB, STD_BB),
                       currentPosition(""),
                       channel("AAPL"),
                       dbFileName("AAPL_15MIN.db"),
                       positionOpen(false) {}

    std::atomic<bool> isRunning;
    int period_BB;
    int STD_BB;
    int period_RSI;
    RSI rsi;
    BollingerBands bb;
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
                rsi.addData(barToOHLCV(bar));
                bb.addData(barToOHLCV(bar));
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
        std::cout << "Historical Data Size: " << historicalData.size() << std::endl;
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
                auto marketClockInfo = orderManager.getMarketInfo();

                if (marketClockInfo["is_open"] == "true")
                {
                    processTrading();
                }
                else
                {
                    std::cout << "Market is closed. Next open time: " << marketClockInfo["next_open"] << std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(60));
                }
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

            if (oneMinCandles.size() == 3)
            {
                OHLCV fiveMinCandle = buildAggregatedCandle(oneMinCandles);
                oneMinCandles.clear();

                historicalData.push_back(fiveMinCandle);
                rsi.addData(fiveMinCandle);
                bb.addData(fiveMinCandle);
                executeTradingLogic();
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(35));
    }

    void executeTradingLogic()
    {
        SignalResult rsiValue = rsi.calculateSignal();
        double RSI = rsiValue.getSingleValue();
        std::pair<double, double> Bands = bb.calculate();
        double lowerBand = Bands.first;
        double upperBand = Bands.second;
        double currentPrice = historicalData.back().close;

        std::cout << "----------------------------------" << std::endl;
        std::cout << "Current Price: " << currentPrice << std::endl;
        std::cout << "RSI: " << RSI << std::endl;
        std::cout << "Lower Bollinger Band: " << lowerBand << std::endl;
        std::cout << "Upper Bollinger Band: " << upperBand << std::endl;
        std::cout << "Position Open: " << positionOpen << std::endl;
        std::cout << "Current Position: " << currentPosition << std::endl;
        std::cout << "----------------------------------" << std::endl;

        if (!positionOpen && currentPrice < lowerBand && RSI < 30)
        {
            orderManager.createOrder("buy", "market", "ioc", "AAPL", "1", std::nullopt, std::nullopt, std::nullopt, std::nullopt);
            currentPosition = "Buy";
            positionOpen = true;
            std::cout << "Buy Signal triggered. Price below lower Bollinger Band and RSI < 30. Order to buy placed successfully." << std::endl;
        }

        else if (positionOpen && currentPrice > upperBand && RSI > 70)
        {
            orderManager.createOrder("sell", "market", "ioc", "AAPL", "1", std::nullopt, std::nullopt, std::nullopt, std::nullopt);
            currentPosition = "Sell";
            positionOpen = false;
            std::cout << "Sell Signal triggered. Price above upper Bollinger Band and RSI > 70. Order to sell placed successfully." << std::endl;
        }
        else
        {
            std::cout << "No action taken. Current signals: Current Price = " << currentPrice << ", RSI = " << RSI
                      << ", Lower Band = " << lowerBand << ", Upper Band = " << upperBand << std::endl;
        }
    }
};
