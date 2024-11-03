#pragma once
#include <torch/torch.h>
#include <vector>
#include <random>
#include <fstream>
#include <sstream>
#include <memory>
#include <sstream>
#include "AlgoEngine-Core/Reinforcement_models/Deep_Q-Learning.hpp"
#include "Utilities/ModelTransfer.hpp"
#include "Utilities/ReplayMemory.hpp"
#include <torch/torch.h>
#include <fstream>
#include <memory>

class Agent
{
public:
    Agent(int replay_mem_size = 10000,
          int batch_size = 40,
          double gamma = 0.98,
          double eps_start = 1.0,
          double eps_end = 0.12,
          int eps_steps = 300,
          double learning_rate = 0.001,
          int input_dim = 24,
          int action_number = 3,
          bool is_double_dqn = true);

    torch::Tensor select_action(torch::Tensor state);
    void optimize_model();
    void store_transition(torch::Tensor state, torch::Tensor action,
                          torch::Tensor next_state, torch::Tensor reward);
    void update_target_network();

private:
    void optimize_double_dqn(const torch::Tensor &states, torch::Tensor &next_state_values,
                             const std::vector<bool> &non_final_mask,
                             const std::vector<torch::Tensor> &next_states);
    void optimize_regular_dqn(torch::Tensor &next_state_values,
                              const std::vector<bool> &non_final_mask,
                              const std::vector<torch::Tensor> &next_states);

    int replay_mem_size;
    int batch_size;
    double gamma;
    double eps_start;
    double eps_end;
    int eps_steps;
    std::unique_ptr<ConvDuelingDQN> policy_net;
    std::unique_ptr<ConvDuelingDQN> target_net;
    torch::optim::Adam optimizer;
    ReplayMemory memory;
    std::mt19937 rng;
    int steps_done;
    bool is_double_dqn;
    torch::Device device;
};
