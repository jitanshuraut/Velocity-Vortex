#ifndef ORDER_H
#define ORDER_H

#include <cstdint>
#include <stdexcept>
#include <memory>

enum class OrderType
{
    GoodTillCancel,
    FillAndKill
};

enum class Side
{
    Buy,
    Sell
};

using Price = std::int32_t;
using Quantity = std::uint32_t;
using OrderId = std::uint64_t;

class Order_
{
public:
    Order_(OrderType orderType, OrderId orderId, Side side, Price price, Quantity quantity);
    OrderId GetOrderId() const;
    OrderType GetOrderType() const;
    Side GetSide() const;
    Price GetPrice() const;
    Quantity GetInitialQuantity() const;
    Quantity GetRemainingQuantity() const;
    Quantity GetFilledQuantity() const;
    bool IsFilled() const;
    void Fill(Quantity quantity);

private:
    OrderType orderType_;
    OrderId orderId_;
    Side side_;
    Price price_;
    Quantity initialQuantity_;
    Quantity remainingQuantity_;
};

using OrderPointer = std::shared_ptr<Order_>;

#endif