#ifndef Agent_HPP
#define Agent_HPP

#include "AlgoEngine-Core/Reinforcement_models/QNetwork.hpp"
#include <vector>
#include <memory>

class Agent
{
public:
    Agent(double alpha, double gamma, double epsilon);
    void train(const std::vector<std::vector<double>> &market_data, double &total_balance);

private:
    torch::Tensor get_state(const std::vector<double> &market_data);
    int choose_action(const torch::Tensor &state);
    void update_q_network(const torch::Tensor &state, int action, double reward, const torch::Tensor &next_state);
    void update_epsilon();
    void execute_action(int action, double &total_balance, const std::vector<double> &bids, const std::vector<double> &asks);

    double alpha_;
    double gamma_;
    double epsilon_;
    double epsilon_min_ = 0.01;
    double epsilon_decay_ = 0.995;
    std::shared_ptr<QNetwork> model_;
    torch::optim::Adam optimizer_;
    torch::Device device_;
};

#endif
