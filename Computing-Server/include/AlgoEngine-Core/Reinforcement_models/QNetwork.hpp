#ifndef QNETWORK_HPP
#define QNETWORK_HPP

#include <torch/torch.h>

const int STATE_SIZE = 300;
const int NUM_ACTIONS = 3;

class QNetwork : public torch::nn::Module
{
public:
    QNetwork();
    torch::Tensor forward(torch::Tensor x);

private:
    torch::nn::Linear layer1{nullptr}, layer2{nullptr}, layer3{nullptr};
};

#endif
