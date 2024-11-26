#include "./Velocity-Bot/strategies/Trend-Following-Strategy.cpp"
#include <chrono>
#include "AlgoEngine-Core/Reinforcement_models/Agent_QNetwork.hpp"
#include <thread>
#include "Utilities/Logger.hpp"
#include <iostream>
#include "Data-Fetcher-Core/DataFetcher-Sub.hpp"

int main()
{
   
    auto logger = Logger::getInstance();
    logger->publishLogMessage("Trading engine starting...", {"Log"});

    // ----------------------------------------------------------------------------------------------

    //   logger->publishLogMessage("starting Testing 1",{"Log"});
    //   logger->publishLogMessage("starting Testing 2",{"Log"});
    //   logger->publishLogMessage("starting Testing 3",{"Log"});

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

    // BOT_Trend_Following tradingBot;

    // try
    // {
    //     tradingBot.initialize();
    // }
    // catch (const std::exception &e)
    // {
    //     std::cerr << "Error during initialization: " << e.what() << std::endl;
    //     return 1;
    // }

    // std::thread tradingThread([&tradingBot]()
    //                           {
    //     try {
    //         tradingBot.run();
    //     } catch (const std::exception& e) {
    //         std::cerr << "Error during run: " << e.what() << std::endl;
    //     } });

    // std::this_thread::sleep_for(std::chrono::minutes(10));
    // tradingBot.stop();

    // if (tradingThread.joinable())
    // {
    //     tradingThread.join();
    // }
    //------------------------------------------------------------------------------------------------

    // Agent agent(
    //     /*replay_mem_size=*/10000,
    //     /*batch_size=*/40,
    //     /*gamma=*/0.98,
    //     /*eps_start=*/1.0,
    //     /*eps_end=*/0.12,
    //     /*eps_steps=*/300,
    //     /*learning_rate=*/0.001,
    //     /*input_dim=*/24,
    //     /*action_number=*/3,
    //     /*is_double_dqn=*/true);

    // std::vector<float> episode_rewards;
    // float epsilon = 1.0;
    // for (int episode = 0; episode < 1000; ++episode)
    // {
    //     float episode_reward = 0.0;
    //     torch::Tensor state = torch::randn({24});
    //     for (int t = 0; t < 100; ++t)
    //     {
    //         torch::Tensor action = agent.select_action(state);
    //         torch::Tensor next_state = torch::randn({24});
    //         float action_value = action.item<float>();
    //         float optimal_action = 1.0;
    //         float reward_value = -std::abs(action_value - optimal_action);
    //         torch::Tensor reward = torch::tensor({reward_value});
    //         episode_reward += reward_value;
    //         agent.store_transition(state, action, next_state, reward);
    //         state = next_state;
    //         agent.optimize_model();
    //         bool done = (t == 99);
    //         if (done)
    //         {
    //             agent.update_target_network();
    //             break;
    //         }
    //     }
    //     episode_rewards.push_back(episode_reward);
    //     if (episode % 10 == 0)
    //     {
    //         float avg_reward = std::accumulate(
    //                                episode_rewards.end() - std::min(10, (int)episode_rewards.size()),
    //                                episode_rewards.end(), 0.0f) /
    //                            std::min(10, (int)episode_rewards.size());

    //         std::cout << "Episode: " << episode
    //                   << " | Avg Reward: " << avg_reward << std::endl;
    //     }
    // }

    //------------------------------------------------------------------------------------------------

    // double alpha = 0.001;
    // double gamma = 0.99;
    // double epsilon = 1.0;

    // Agent agent(alpha, gamma, epsilon);

    // std::vector<std::vector<double>> market_data;
    // for (int i = 0; i < 10000; ++i)
    // {
    //     std::vector<double> data(STATE_SIZE);
    //     for (int j = 0; j < STATE_SIZE; ++j)
    //     {
    //         data[j] = static_cast<double>(rand()) / RAND_MAX;
    //     }
    //     market_data.push_back(data);
    // }

    // double total_balance = 1000.0;
    // agent.train(market_data, total_balance);
    // std::cout << "Final Balance: " << total_balance << std::endl;

    return 0;
}