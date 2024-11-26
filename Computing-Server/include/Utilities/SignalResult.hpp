#ifndef SIGNALRESULT_HPP
#define SIGNALRESULT_HPP

#include <variant>
#include <vector>
#include <stdexcept>
#include <iostream>

class SignalResult
{
public:
    using ResultType = std::variant<double, std::vector<double>>;

    SignalResult(double singleValue) : value(singleValue) {}
    SignalResult(const std::vector<double> &vectorValue) : value(vectorValue) {}

    SignalResult(const SignalResult &other) = default;

    SignalResult &operator=(const SignalResult &other) = default;

    bool isSingleValue() const
    {
        return std::holds_alternative<double>(value);
    }

    double getSingleValue() const
    {
        if (isSingleValue())
        {
            return std::get<double>(value);
        }
        std::cerr << "Error: SignalResult does not hold a single value (double)." << std::endl;
        throw std::bad_variant_access();
    }

    std::vector<double> getVectorValue() const
    {
        if (!isSingleValue())
        {
            return std::get<std::vector<double>>(value);
        }
        throw std::bad_variant_access();
    }

private:
    ResultType value;
};

#endif