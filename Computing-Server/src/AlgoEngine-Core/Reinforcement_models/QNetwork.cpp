#include "AlgoEngine-Core/Reinforcement_models/QNetwork.hpp"

QNetwork::QNetwork()
{
    layer1 = register_module("layer1", torch::nn::Linear(STATE_SIZE, 64));
    layer2 = register_module("layer2", torch::nn::Linear(64, 64));
    layer3 = register_module("layer3", torch::nn::Linear(64, NUM_ACTIONS));
}

torch::Tensor QNetwork::forward(torch::Tensor x)
{
    x = torch::relu(layer1->forward(x));
    x = torch::relu(layer2->forward(x));
    return layer3->forward(x);
}
