#include "AlgoEngine-Core/Reinforcement_models/Deep_Q-Learning.hpp"


ConvDQN::ConvDQN(int input_dim, int action_number)
    : input_dimension(input_dim), action_number(action_number)
{
  
    layers = register_module("layers",
                             torch::nn::Sequential(
                                 torch::nn::Conv1d(torch::nn::Conv1dOptions(1, 32, 3).stride(1)),
                                 torch::nn::ReLU(),
                                 torch::nn::Conv1d(torch::nn::Conv1dOptions(32, 64, 3).stride(1)),
                                 torch::nn::ReLU(),
                                 torch::nn::Flatten(),
                                 torch::nn::Linear(64 * (input_dim - 4), 120),
                                 torch::nn::ReLU(),
                                 torch::nn::Linear(120, action_number)));
}


torch::Tensor ConvDQN::forward(torch::Tensor x)
{
    return layers->forward(x);
}


ConvDuelingDQN::ConvDuelingDQN(int input_dim, int action_number)
    : input_dimension(input_dim), action_number(action_number)
{
    
    value_stream = register_module("value_stream",
                                   torch::nn::Sequential(
                                       torch::nn::Conv1d(torch::nn::Conv1dOptions(1, 32, 3).stride(1)),
                                       torch::nn::ReLU(),
                                       torch::nn::Conv1d(torch::nn::Conv1dOptions(32, 64, 3).stride(1)),
                                       torch::nn::ReLU(),
                                       torch::nn::Flatten(),
                                       torch::nn::Linear(64 * (input_dim - 4), 120),
                                       torch::nn::ReLU(),
                                       torch::nn::Linear(120, 1)));


    advantage_stream = register_module("advantage_stream",
                                       torch::nn::Sequential(
                                           torch::nn::Conv1d(torch::nn::Conv1dOptions(1, 32, 3).stride(1)),
                                           torch::nn::ReLU(),
                                           torch::nn::Conv1d(torch::nn::Conv1dOptions(32, 64, 3).stride(1)),
                                           torch::nn::ReLU(),
                                           torch::nn::Flatten(),
                                           torch::nn::Linear(64 * (input_dim - 4), 120),
                                           torch::nn::ReLU(),
                                           torch::nn::Linear(120, action_number)));
}


torch::Tensor ConvDuelingDQN::forward(torch::Tensor x)
{
    auto value = value_stream->forward(x);
    auto advantage = advantage_stream->forward(x);
    return value + (advantage - advantage.mean(1, true));
}
