#include "AlgoEngine-Core/Reinforcement_models/Agent_QNetwork.hpp"
#include <iostream>
#include <random>

const double TRANSACTION_FEE = 0.001;
const double MAX_RISK = 0.05;

Agent::Agent(double alpha, double gamma, double epsilon)
    : alpha_(alpha), gamma_(gamma), epsilon_(epsilon),
      model_(std::make_shared<QNetwork>()),
      device_(torch::cuda::is_available() ? torch::kCUDA : torch::kCPU),
      optimizer_(model_->parameters(), torch::optim::AdamOptions(alpha))
{
    model_->to(device_);
}

torch::Tensor Agent::get_state(const std::vector<double> &market_data)
{

    auto state = torch::from_blob(const_cast<double *>(market_data.data()),
                                  {1, STATE_SIZE},
                                  torch::kFloat32)
                     .clone();
    return state.to(device_);
}

int Agent::choose_action(const torch::Tensor &state)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    if (dis(gen) < epsilon_)
    {
        std::uniform_int_distribution<> action_dist(0, NUM_ACTIONS - 1);
        return action_dist(gen);
    }
    else
    {
        torch::NoGradGuard no_grad;
        auto q_values = model_->forward(state);
        return std::get<1>(q_values.max(1)).item<int>();
    }
}

void Agent::update_q_network(const torch::Tensor &state,
                             int action,
                             double reward,
                             const torch::Tensor &next_state)
{

    torch::Tensor next_q_values = model_->forward(next_state);
    double max_next_q = std::get<0>(next_q_values.max(1)).item<float>();
    double target = reward + gamma_ * max_next_q;

    torch::Tensor current_q_values = model_->forward(state);
    torch::Tensor action_tensor = torch::tensor({action}, device_);
    torch::Tensor current_q = current_q_values.gather(1, action_tensor.unsqueeze(0));

    torch::Tensor loss = torch::mse_loss(current_q, torch::tensor({target}, device_));
    optimizer_.zero_grad();
    loss.backward();
    optimizer_.step();
}

void Agent::update_epsilon()
{
    epsilon_ = std::max(epsilon_min_, epsilon_ * epsilon_decay_);
}

void Agent::execute_action(int action, double &total_balance,
                           const std::vector<double> &bids,
                           const std::vector<double> &asks)
{

    switch (action)
    {
    case 0:
        if (total_balance > 0)
        {
            double price = asks[0] * (1 - TRANSACTION_FEE);
            double amount = std::max(MAX_RISK / price, 0.001);
            total_balance -= (price * amount);
        }
        break;
    case 1:
        if (total_balance > 0)
        {
            double price = bids[0] * (1 + TRANSACTION_FEE);
            double amount = std::max(MAX_RISK / price, 0.001);
            total_balance += (price * amount);
        }
        break;
    case 2:
        break;
    }
}

void Agent::train(const std::vector<std::vector<double>> &market_data,
                  double &total_balance)
{
    for (size_t i = 0; i < market_data.size() - 1; ++i)
    {

        auto state = get_state(market_data[i]);

        int action = choose_action(state);
        std::vector<double> bids(market_data[i].begin(), market_data[i].begin() + 6);
        std::vector<double> asks(market_data[i].begin() + 6, market_data[i].begin() + 12);

        double initial_balance = total_balance;
        execute_action(action, total_balance, bids, asks);
        double reward = total_balance - initial_balance;
        auto next_state = get_state(market_data[i + 1]);

        update_q_network(state, action, reward, next_state);
        update_epsilon();

        if (i % 100 == 0)
        {
            std::cout << "Iteration: " << i
                      << ", Epsilon: " << epsilon_
                      << ", Balance: " << total_balance << std::endl;
        }
    }
}
