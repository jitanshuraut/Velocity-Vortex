#ifndef ORDER_DETAILS_HPP
#define ORDER_DETAILS_HPP

#include <string>
#include <optional>

struct OrderDetails
{
    std::string side;
    std::string type;
    std::string timeInForce;
    std::string symbol;
    std::string qty;
    std::optional<std::string> limitPrice;
    std::optional<std::string> stopPrice;
    std::optional<std::string> trailPrice;
    std::optional<std::string> trailPercent;
};

#endif