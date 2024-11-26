#include "AlgoEngine-Core/Reinforcement_models/Agent_Deep_Q-Learning.hpp"
#include <random>
#include <algorithm>
#include <deque>
#include <torch/torch.h>

void Agent::store_transition(torch::Tensor state, torch::Tensor action,
                             torch::Tensor next_state, torch::Tensor reward)
{
    memory.push(Transition(state, action, next_state, reward));
}

void Agent::update_target_network()
{
    policy_net->to(device);
    target_net->to(device);
    ModelTransfer::transfer_parameters_in_memory(*policy_net, *target_net, device);
}

Agent::Agent(int replay_mem_size,
             int batch_size,
             double gamma,
             double eps_start,
             double eps_end,
             int eps_steps,
             double learning_rate,
             int input_dim,
             int action_number,
             bool is_double_dqn)
    : replay_mem_size(replay_mem_size),
      batch_size(batch_size),
      gamma(gamma),
      eps_start(eps_start),
      eps_end(eps_end),
      eps_steps(eps_steps),
      policy_net(std::make_unique<ConvDuelingDQN>(input_dim, action_number)),
      target_net(std::make_unique<ConvDuelingDQN>(input_dim, action_number)),
      optimizer(policy_net->parameters(), torch::optim::AdamOptions(learning_rate)),
      memory(replay_mem_size),
      rng(std::random_device{}()),
      steps_done(0),
      is_double_dqn(is_double_dqn),
      device(torch::cuda::is_available() ? torch::kCUDA : torch::kCPU)
{
    policy_net->to(device);
    target_net->to(device);
    ModelTransfer::transfer_parameters_in_memory(*policy_net, *target_net, device);
    target_net->eval();
}

torch::Tensor Agent::select_action(torch::Tensor state)
{
    state = state.unsqueeze(0).unsqueeze(1).to(device);
    std::uniform_real_distribution<> dist(0, 1);

    double eps_threshold = steps_done > eps_steps ? eps_end : eps_start - (steps_done * (eps_start - eps_end) / eps_steps);

    steps_done++;

    if (dist(rng) > eps_threshold)
    {
        torch::NoGradGuard no_grad;
        auto action = policy_net->forward(state).argmax(1);
        return action.to(device);
    }
    else
    {
        std::uniform_int_distribution<> action_dist(0, 2);
        return torch::tensor({action_dist(rng)}, device);
    }
}

void Agent::optimize_model()
{
    if (memory.size() < batch_size)
        return;

    auto batch = memory.sample(batch_size);

    std::vector<torch::Tensor> state_batch, action_batch, reward_batch, next_state_batch;
    std::vector<bool> non_final_mask;

    for (const auto &transition : batch)
    {
        state_batch.push_back(transition.state);
        action_batch.push_back(transition.action);
        reward_batch.push_back(transition.reward);
        next_state_batch.push_back(transition.next_state);
        non_final_mask.push_back(transition.next_state.defined());
    }

    auto states = torch::stack(state_batch).to(device).unsqueeze(1); 
    auto actions = torch::stack(action_batch).to(device).squeeze();  
    auto rewards = torch::stack(reward_batch).to(device).squeeze();  


    auto q_values = policy_net->forward(states); 
    auto state_action_values = q_values.gather(1, actions.unsqueeze(1)); 
    auto next_state_values = torch::zeros({batch_size}, device);

    if (is_double_dqn)
    {
        optimize_double_dqn(states, next_state_values, non_final_mask, next_state_batch);
    }
    else
    {
        optimize_regular_dqn(next_state_values, non_final_mask, next_state_batch);
    }
    auto expected_state_action_values = (next_state_values * gamma) + rewards; 
    expected_state_action_values = expected_state_action_values.unsqueeze(1);       // Shape: [40, 1]
    auto loss = torch::mse_loss(state_action_values, expected_state_action_values); // Shape: [40, 1]

    optimizer.zero_grad();
    loss.backward();

    for (auto &param : policy_net->parameters())
    {
        param.grad().data().clamp_(-1, 1);
    }

    optimizer.step();
}

void Agent::optimize_double_dqn(const torch::Tensor &states,
                                torch::Tensor &next_state_values,
                                const std::vector<bool> &non_final_mask,
                                const std::vector<torch::Tensor> &next_states)
{
    auto policy_output = policy_net->forward(states);
    auto [_, next_state_actions] = policy_output.max(1);
    next_state_actions = next_state_actions.to(torch::kLong);

    std::vector<torch::Tensor> non_final_next_states;
    for (size_t i = 0; i < non_final_mask.size(); i++)
    {
        if (non_final_mask[i])
        {
            non_final_next_states.push_back(next_states[i]);
        }
    }

    if (!non_final_next_states.empty())
    {
        auto non_final_states_tensor = torch::stack(non_final_next_states).to(device).unsqueeze(1);
        auto target_values = target_net->forward(non_final_states_tensor);

        size_t idx = 0;
        for (size_t i = 0; i < non_final_mask.size(); i++)
        {
            if (non_final_mask[i])
            {
                auto action_idx = next_state_actions[i].unsqueeze(0);
                next_state_values[i] = target_values[idx].gather(0, action_idx).squeeze();
                idx++;
            }
        }
    }
}

void Agent::optimize_regular_dqn(torch::Tensor &next_state_values,
                                 const std::vector<bool> &non_final_mask,
                                 const std::vector<torch::Tensor> &next_states)
{
    std::vector<torch::Tensor> non_final_next_states;
    for (size_t i = 0; i < non_final_mask.size(); i++)
    {
        if (non_final_mask[i])
        {
            non_final_next_states.push_back(next_states[i]);
        }
    }

    if (!non_final_next_states.empty())
    {
        auto non_final_states_tensor = torch::stack(non_final_next_states).to(device).unsqueeze(1);
        auto [next_state_values_tensor, _] = target_net->forward(non_final_states_tensor).max(1);

        size_t idx = 0;
        for (size_t i = 0; i < non_final_mask.size(); i++)
        {
            if (non_final_mask[i])
            {
                next_state_values[i] = next_state_values_tensor[idx];
                idx++;
            }
        }
    }
}