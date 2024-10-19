#include "Orderbook/order_book.hpp"
#include <cmath>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <iostream>
#include <memory>
#include "Orderbook/transactions.hpp"
#include "Orderbook/order_type.hpp"
#include "Orderbook/order_status.hpp"
#include "Orderbook/order_orderBook.hpp"
#include "Orderbook/limit_order.hpp"
#include "Orderbook/order_tree.hpp"
using std::cout;
using std::endl;
using std::ostringstream;
using std::shared_ptr;

OrderTree &OrderBook::getBuyTree()
{
    return buy_tree;
}

OrderTree &OrderBook::getSellTree()
{
    return sell_tree;
}

OrderTree &OrderBook::getOrderTreeByOrderType(const OrderType &order_type)
{

    return (order_type == OrderType::BUY) ? buy_tree : sell_tree;
}

const int &OrderBook::getTotalOrdersSubmitted()
{
    return total_orders_submitted;
}

const int &OrderBook::getTotalOrdersFulfilled()
{
    return total_orders_submitted - (buy_tree.getLength() + sell_tree.getLength());
}

const int &OrderBook::getTotalOrdersUnfulfilled()
{
    return (buy_tree.getLength() + sell_tree.getLength());
}

string OrderBook::getResults()
{
    return output_data.str();
}

void OrderBook::processOrder(Order &incoming_order)
{

    total_orders_submitted++;
    incoming_order.storeOrderInfo(output_data, "\033[0m");

    OrderTree &matching_tree = (incoming_order.getType() == OrderType::BUY) ? sell_tree : buy_tree;
    matching_tree.matchPriceOrder(incoming_order, output_data);
    if (incoming_order.getStatus() == OrderStatus::FULFILLED)
    {

        incoming_order.storeOrderInfo(output_data, "\033[32m");
    }

    else
    {
        incoming_order.setOrderStatus(OrderStatus::STORED);
        incoming_order.storeOrderInfo(output_data, "\033[33m");
        OrderTree &storing_tree = (incoming_order.getType() == OrderType::BUY) ? buy_tree : sell_tree;
        storing_tree.addPriceOrder(incoming_order);
    }

    if (incoming_order.getID() == 75000)
    {
        getCurrentSnapshot();
    }

    return;
}

void OrderBook::cancelOrder(const std::shared_ptr<Order> &cancel_order)
{
    getOrderTreeByOrderType(cancel_order->getType()).deletePriceOrder(cancel_order);
}

void OrderBook::getCurrentSnapshot()
{

    std::vector<std::pair<float, float>> sorted_buys(buy_tree.order_tree.begin(), buy_tree.order_tree.end());
    std::sort(sorted_buys.begin(), sorted_buys.end(), [](const auto &b1, const auto &b2)
              { return b1.first > b2.first; });

    for (size_t i = 1; i < sorted_buys.size(); ++i)
    {
        sorted_buys[i].second += sorted_buys[i - 1].second;
    }

    std::ofstream out_buy_prices("output_buy_prices.txt");

    for (const auto &buy : sorted_buys)
    {
        out_buy_prices << buy.first << ",";
    }
    out_buy_prices << std::endl;

    out_buy_prices.close();

    std::ofstream out_buy_sizes("output_buy_sizes.txt");
    for (const auto &buy : sorted_buys)
    {
        out_buy_sizes << buy.second << ",";
    }
    out_buy_sizes << std::endl;

    out_buy_sizes.close();
    std::vector<std::pair<float, float>> sorted_sells(sell_tree.order_tree.begin(), sell_tree.order_tree.end());
    std::sort(sorted_sells.begin(), sorted_sells.end(), [](const auto &b1, const auto &b2)
              { return b1.first < b2.first; });

    for (size_t i = 1; i < sorted_sells.size(); ++i)
    {
        sorted_sells[i].second += sorted_sells[i - 1].second;
    }

    std::ofstream out_sell_prices("output_sell_prices.txt");
    for (const auto &sell : sorted_sells)
    {
        out_sell_prices << sell.first << ",";
    }
    out_sell_prices << std::endl;

    out_sell_prices.close();

    std::ofstream out_sell_sizes("output_sell_sizes.txt");
    for (const auto &sell : sorted_sells)
    {
        out_sell_sizes << sell.second << ",";
    }
    out_sell_sizes << std::endl;

    out_sell_sizes.close();
}