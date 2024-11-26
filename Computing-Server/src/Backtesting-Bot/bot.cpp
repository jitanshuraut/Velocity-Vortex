#include "matplotlibcpp.h"
#include "Velocity-Bot/bot.hpp"
#include "AlgoEngine-Core/algorithm/SMA.hpp"
#include "AlgoEngine-Core/algorithm/EMA.hpp"
#include "Order-Manager-Core/order_manager.hpp"
#include "IO-Broker-Core/RedisDBManager.hpp"
#include "Data-Fetcher-Core/DataFetcher.hpp"
#include "Utilities/Bar.hpp"
#include "Utilities/OHLCV.hpp"
#include "Utilities/Utilities.hpp"
#include <thread>
#include <chrono>
#include <atomic>
#include <ctime>

namespace plt = matplotlibcpp;

class BackTester : public Bot {
public:
    BackTester() 
        : isRunning(false),
          Amount(1000),
          Holding(0),
          period_SMA(12),
          period_EMA(12),
          sma("Simple Moving Average", period_SMA),
          ema("Exponential Moving Average", period_EMA),
          currentPosition(""),
          positionOpen(false) {}

    std::atomic<bool> isRunning;
    int period_SMA;
    int period_EMA;
    int Amount;
    int Holding;
    SMA sma;
    EMA ema;
    DataFetcher fetcher;
    OrderManager orderManager;
    RedisDBManager publisher;
    std::vector<OHLCV> historicalData;
    std::vector<Bar> oneMinCandles;
    std::string currentPosition;
    bool positionOpen;
    std::vector<double> buyPoints;
    std::vector<double> sellPoints;
    std::vector<double> timeIndex;
    std::vector<double> priceData;

    void initialize() override {
        try {
            std::string jsonResponseBars = fetcher.getHistoricalBars("AAPL", "1Day", "2022-01-01", "2022-01-31");
            std::vector<Bar> Bars = fetcher.parseHistoricalBars(jsonResponseBars, "AAPL");

            for (const auto& bar : Bars) {
                historicalData.push_back(barToOHLCV(bar));
                sma.addData(barToOHLCV(bar));
                ema.addData(barToOHLCV(bar));
                timeIndex.push_back(convertToTimestamp(bar.time));
                priceData.push_back(bar.close);
                buyPoints.push_back(NAN);
                sellPoints.push_back(NAN);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error during initialization: " << e.what() << std::endl;
        }

        std::cout << "Initialize Done" << std::endl;
        std::cout << "----------------------------------" << std::endl;
        std::cout << "SMA Period: " << sma.getPeriod() << std::endl;
        std::cout << "Historical Data Size: " << sma.getDataSize() << std::endl;
        std::cout << "Signal Size: " << sma.getSignalSize() << std::endl;
        std::cout << "----------------------------------" << std::endl;
    }

    void run() override {
        std::cout << "Running" << std::endl;
        std::cout << "----------------------------------" << std::endl;

        std::string jsonResponseBars = fetcher.getHistoricalBars("AAPL", "1Day", "2022-02-01", "2024-09-15");
        std::vector<Bar> Bars = fetcher.parseHistoricalBars(jsonResponseBars, "AAPL");

        for (const auto& bar : Bars) {
            historicalData.push_back(barToOHLCV(bar));
            timeIndex.push_back(convertToTimestamp(bar.time));
            priceData.push_back(bar.close);
            buyPoints.push_back(NAN);
            sellPoints.push_back(NAN);

            try {
                processTrading();
            } catch (const std::exception& e) {
                std::cerr << "Error during run loop: " << e.what() << std::endl;
            }
        }

        plotResults();
    }

    void stop() override {
        isRunning = false;
        std::cout << "Bot stopping..." << std::endl;
    }

    bool isRunningState() const {
        return isRunning.load();
    }

private:
    void processTrading() {
        std::cout << "Market is open. Proceeding with trading logic..." << std::endl;

        sma.addData(historicalData.back());
        ema.addData(historicalData.back());
        executeTradingLogic(historicalData.back().close);
    }

    void executeTradingLogic(double price) {
        SignalResult smaValue = sma.calculateSignal();
        SignalResult emaValue = ema.calculateSignal();


        double longSMA = smaValue.getSingleValue();  
        double shortEMA = emaValue.getSingleValue(); 

        std::cout << "----------------------------------" << std::endl;
        std::cout << "Short EMA: " << shortEMA << std::endl;
        std::cout << "Long SMA: " << longSMA << std::endl;
        std::cout << "Position Open: " << positionOpen << std::endl;
        std::cout << "Current Position: " << currentPosition << std::endl;
        std::cout << "----------------------------------" << std::endl;

        int currentIndex = timeIndex.size() - 1;

        if (!positionOpen && shortEMA > longSMA) {
            // Buy action
            Holding += (Amount / price);  
            Amount -= price * Holding;
            currentPosition = "Buy";
            positionOpen = true;

            buyPoints[currentIndex] = price;
            std::cout << "Buy Signal triggered. Order to buy placed successfully." << std::endl;
        } else if (positionOpen && shortEMA < longSMA) {
            // Sell action
            Amount += price * Holding;
            Holding = 0;  
            currentPosition = "Sell";
            positionOpen = false;

            sellPoints[currentIndex] = price;
            std::cout << "Sell Signal triggered. Order to sell placed successfully." << std::endl;
        } else {
            std::cout << "No action taken. Current signals: Short EMA = " << shortEMA << ", Long SMA = " << longSMA << std::endl;
        }
    }

    void plotResults() {
        try {
            
             plt::figure_size(1200, 800);
            plt::plot(priceData, {{"label", "Price"}});


            std::vector<double> buyX, buyY;
            for (size_t i = 0; i < buyPoints.size(); ++i) {
                if (!std::isnan(buyPoints[i])) {
                    buyX.push_back(i); 
                    buyY.push_back(buyPoints[i]);
                }
            }
            plt::plot(buyX, buyY, "g^");

            
            std::vector<double> sellX, sellY;
            for (size_t i = 0; i < sellPoints.size(); ++i) {
                if (!std::isnan(sellPoints[i])) {
                    sellX.push_back(i); 
                    sellY.push_back(sellPoints[i]);
                }
            }
            plt::plot(sellX, sellY, "rv");


            std::vector<double> xticks;
            std::vector<std::string> xtickLabels;
            size_t step = timeIndex.size() / 10; 
            for (size_t i = 0; i < timeIndex.size(); i += step) {
                xticks.push_back(i); // Use index for x-ticks
                xtickLabels.push_back(timestampToString(timeIndex[i])); 
            }
            plt::xticks(xticks, xtickLabels); 


            plt::xlabel("Time");
            plt::ylabel("Price");
            plt::title("AAPL Stock Price with Buy/Sell Signals");


            plt::show();
        } catch (const std::exception& e) {
            std::cerr << "Error in plotting: " << e.what() << std::endl;
        }
    }

    double convertToTimestamp(const std::string& dateStr) {
        std::tm tm = {};
        std::istringstream ss(dateStr);
        ss >> std::get_time(&tm, "%Y-%m-%d");
        return std::mktime(&tm);
    }

    std::string timestampToString(double timestamp) {
        std::time_t time = static_cast<std::time_t>(timestamp);
        std::tm* tm = std::localtime(&time);
        char buffer[11];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", tm);
        return std::string(buffer);
    }
};
