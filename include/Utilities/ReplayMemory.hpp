#pragma once
#include <fstream>
#include <memory>
#include <torch/torch.h>
#include <vector>
#include <random>
#include <fstream>
#include <sstream>
#include <memory>
#include <sstream>

struct Transition
{
    torch::Tensor state;
    torch::Tensor action;
    torch::Tensor next_state;
    torch::Tensor reward;

    Transition(torch::Tensor s, torch::Tensor a, torch::Tensor ns, torch::Tensor r)
        : state(s), action(a), next_state(ns), reward(r) {}
};

class ReplayMemory
{
public:
    explicit ReplayMemory(size_t capacity) : capacity(capacity) {}

    void push(const Transition &transition)
    {
        if (memory.size() >= capacity)
        {
            memory.pop_front();
        }
        memory.push_back(transition);
    }

    std::vector<Transition> sample(size_t batch_size)
    {
        std::vector<Transition> batch;
        std::random_device rd;
        std::mt19937 gen(rd());

        for (size_t i = 0; i < batch_size && i < memory.size(); ++i)
        {
            std::uniform_int_distribution<> dis(0, memory.size() - 1);
            batch.push_back(memory[dis(gen)]);
        }
        return batch;
    }

    size_t size() const { return memory.size(); }

private:
    size_t capacity;
    std::deque<Transition> memory;
};