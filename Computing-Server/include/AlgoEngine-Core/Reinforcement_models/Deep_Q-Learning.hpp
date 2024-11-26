#pragma once
#ifndef CONV_NETWORKS_HPP
#define CONV_NETWORKS_HPP

#include <torch/torch.h>

class ConvDQN : public torch::nn::Module
{
private:
    torch::nn::Sequential layers{nullptr};
    int input_dimension;
    int action_number;

public:
    ConvDQN(int input_dim, int action_number);

    torch::Tensor forward(torch::Tensor x);
};

class ConvDuelingDQN : public torch::nn::Module
{
private:
    torch::nn::Sequential value_stream{nullptr};
    torch::nn::Sequential advantage_stream{nullptr};
    int input_dimension;
    int action_number;

public:
    ConvDuelingDQN(int input_dim, int action_number);

    torch::Tensor forward(torch::Tensor x);
};

#endif
