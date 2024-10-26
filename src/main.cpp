#include "./Velocity-Bot/strategies/Reversal-Strategy-Fibonacci-Retracement.cpp"
#include <chrono>
#include <thread>
#include <iostream>

int main()
{
    // ----------------------------------------------------------------------------------------------

    // For WebSocket-based bots

    // net::io_context ioc;
    // ssl::context ctx{ssl::context::tlsv12_client};
    // ctx.set_options(
    //     ssl::context::default_workarounds |
    //     ssl::context::no_sslv2 |
    //     ssl::context::no_sslv3);

    // auto bot = std::make_shared<TestBOT>(ioc, ctx);
    // bot->initialize();
    // std::thread ioc_thread([&ioc]()
    //                        {
    //     try {
    //         ioc.run();
    //     } catch (const std::exception& e) {
    //         std::cerr << "IO Context error: " << e.what() << std::endl;
    //     } });

    // bot->run();

    // ioc_thread.join();

    // ----------------------------------------------------------------------------------------------

    // For candle-based bots

    Reversal_Fibonacci_Retracement tradingBot;

    try
    {
        tradingBot.initialize();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error during initialization: " << e.what() << std::endl;
        return 1;
    }

    std::thread tradingThread([&tradingBot]()
                              {
        try {
            tradingBot.run(); 
        } catch (const std::exception& e) {
            std::cerr << "Error during run: " << e.what() << std::endl;
        } });

    std::this_thread::sleep_for(std::chrono::minutes(10));
    tradingBot.stop();

    if (tradingThread.joinable())
    {
        tradingThread.join();
    }

    return 0;
}