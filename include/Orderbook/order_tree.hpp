#ifndef ORDER_TREE_H_
#define ORDER_TREE_H_

#pragma once
#include <map>
#include <unordered_map>
#include <memory>
#include <unordered_set>
#include <sstream>
#include "./transactions.hpp"
#include "./order_type.hpp"
#include "./order_status.hpp"
#include "./order_orderBook.hpp"
#include "./limit_order.hpp"

class OrderTree
{
public:
	std::map<float, int> order_tree;

	std::unordered_set<unsigned int> order_set;

	std::unordered_map<float, LimitOrder> price_map;

	float min_price;
	float max_price;
	OrderType type;

public:
	OrderTree(OrderType tree_type) : order_tree({}), order_set({}), price_map({}), min_price(std::numeric_limits<float>::quiet_NaN()), max_price(std::numeric_limits<float>::quiet_NaN()), type(tree_type) {};

	size_t getLength() const;

	const float &getMinPrice();
	void setMinPrice(const float &new_min);

	const float &getMaxPrice();
	void setMaxPrice(const float &new_max);

	const OrderType &getType() const;

	void addPriceOrder(Order &incoming_order);

	void deletePriceOrder(const std::shared_ptr<Order> &order_to_delete);

	void deleteLimitPrice(const float &limit_price);

	void matchPriceOrder(Order &incoming_order, std::ostringstream &store_transaction);
};

#endif