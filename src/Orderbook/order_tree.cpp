#include "Orderbook/order_tree.hpp"
#include <cmath>
#include <sstream>
#include <memory>
#include "Orderbook/transactions.hpp"
#include "Orderbook/order_type.hpp"
#include "Orderbook/order_status.hpp"
#include "Orderbook/order_orderBook.hpp"
#include "Orderbook/limit_order.hpp"
using std::ostringstream;
using std::shared_ptr;

size_t OrderTree::getLength() const
{
    return order_set.size();
}

const float &OrderTree::getMinPrice()
{
    return min_price;
}

void OrderTree::setMinPrice(const float &new_min)
{
    min_price = new_min;
}

const float &OrderTree::getMaxPrice()
{
    return max_price;
}

void OrderTree::setMaxPrice(const float &new_max)
{
    max_price = new_max;
}

const OrderType &OrderTree::getType() const
{
    return type;
}

void OrderTree::addPriceOrder(Order &incoming_order)
{

    order_tree[incoming_order.getPrice()]++;

    price_map[incoming_order.getPrice()].addOrder(incoming_order);

    order_set.insert(incoming_order.getID());

    if (std::isnan(min_price) || min_price > incoming_order.getPrice())
    {
        min_price = incoming_order.getPrice();
    }

    if (std::isnan(max_price) || max_price < incoming_order.getPrice())
    {
        max_price = incoming_order.getPrice();
    }
}

void OrderTree::deletePriceOrder(const std::shared_ptr<Order> &order_to_delete)
{

    if (order_set.erase(order_to_delete->getID()) == 0)
    {
        return;
    }

    price_map[order_to_delete->getPrice()].deleteOrder(order_to_delete);

    if (price_map[order_to_delete->getPrice()].getListLength() <= 0)
    {
        price_map.erase(order_to_delete->getPrice());
    }

    order_tree[order_to_delete->getPrice()]--;

    if (order_tree[order_to_delete->getPrice()] <= 0)
    {
        order_tree.erase(order_to_delete->getPrice());
    }

    if (max_price == order_to_delete->getPrice())
    {

        if (order_tree.empty())
        {
            max_price = std::numeric_limits<float>::quiet_NaN();
        }

        else if (price_map.find(order_to_delete->getPrice()) == price_map.end())
        {
            max_price = order_tree.rbegin()->first;
        }
    }

    if (min_price == order_to_delete->getPrice())
    {

        if (order_tree.empty())
        {
            min_price = std::numeric_limits<float>::quiet_NaN();
        }

        else if (price_map.find(order_to_delete->getPrice()) == price_map.end())
        {
            min_price = order_tree.begin()->first;
        }
    }
}

void OrderTree::deleteLimitPrice(const float &limit_price)
{

    price_map.erase(limit_price);

    order_tree.erase(limit_price);

    if (max_price == limit_price)
    {

        if (order_tree.empty())
        {
            max_price = std::numeric_limits<float>::quiet_NaN();
        }

        else
        {
            max_price = order_tree.rbegin()->first;
        }
    }

    if (min_price == limit_price)
    {

        if (order_tree.empty())
        {
            min_price = std::numeric_limits<float>::quiet_NaN();
        }

        else
        {
            min_price = order_tree.begin()->first;
        }
    }
}

void OrderTree::matchPriceOrder(Order &incoming_order, ostringstream &store_transaction)
{

    if (price_map.empty())
    {
        return;
    }

    if (incoming_order.getType() == type)
    {
        return;
    }

    auto price_to_match = (incoming_order.getType() == OrderType::BUY) ? getMinPrice() : getMaxPrice();
    auto &orderPeakQuantity = incoming_order.getPeakQuantity();
    auto &orderPrice = incoming_order.getPrice();
    auto &orderType = incoming_order.getType();

    while (orderPeakQuantity > 0 &&
           ((orderType == OrderType::BUY && orderPrice >= price_to_match) ||
            (orderType == OrderType::SELL && orderPrice <= price_to_match)))
    {

        LimitOrder &matching_LimitOrder = price_map[price_to_match];
        matching_LimitOrder.matchOrder(incoming_order, order_set, store_transaction);

        if (matching_LimitOrder.getListLength() == 0)
        {
            deleteLimitPrice(price_to_match);

            if (price_map.size() == 0)
            {
                break;
            }

            price_to_match = (incoming_order.getType() == OrderType::BUY) ? getMinPrice() : getMaxPrice();
        }
    }
    return;
}