#include "./Backtesting-Bot/bot.cpp" 
#include <chrono>
#include <thread>
#include <iostream>


int main() {
    BackTester tradingBot;
    
    // try {
    //     tradingBot.initialize();
    // } catch (const std::exception& e) {
    //     std::cerr << "Error during initialization: " << e.what() << std::endl;
    //     return 1; 
    // }

    // std::thread tradingThread([&tradingBot]() {
    //     try {
    //         tradingBot.run(); 
    //     } catch (const std::exception& e) {
    //         std::cerr << "Error during run: " << e.what() << std::endl;
    //     }
    // });

    // std::this_thread::sleep_for(std::chrono::minutes(10));
    // tradingBot.stop(); 

    // if (tradingThread.joinable()) {
    //     tradingThread.join();
    // }
    tradingBot.initialize();
    tradingBot.run();


    return 0;
}
