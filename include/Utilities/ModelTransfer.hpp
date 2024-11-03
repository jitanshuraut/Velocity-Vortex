#pragma once
#include <torch/torch.h>
#include <fstream>
#include <sstream>
#include <memory>
#include <sstream>
#include <memory>

class ModelTransfer
{
public:
    static void transfer_parameters_in_memory(torch::nn::Module &source, torch::nn::Module &target, const torch::Device &device)
    {
        torch::NoGradGuard no_grad;
        auto source_params = source.named_parameters();
        auto target_params = target.named_parameters();

        for (const auto &val : source_params)
        {
            const auto &name = val.key();
            const auto &param = val.value();

            if (target_params.contains(name))
            {
                target_params[name].copy_(param);
            }
        }

        auto source_buffers = source.named_buffers();
        auto target_buffers = target.named_buffers();

        for (const auto &val : source_buffers)
        {
            const auto &name = val.key();
            const auto &buffer = val.value();

            if (target_buffers.contains(name))
            {
                target_buffers[name].copy_(buffer);
            }
        }
    }

    static void save_parameters_to_file(torch::nn::Module &model, const std::string &filename)
    {
        torch::serialize::OutputArchive archive;
        model.save(archive);
        archive.save_to(filename);
    }

    static void load_parameters(torch::nn::Module &model, const std::string &filename)
    {
        torch::serialize::InputArchive archive;
        archive.load_from(filename);
        model.load(archive);
    }
};
