
#ifndef LIMITORDER_H_
#define LIMITORDER_H_

#pragma once
#include <string>
#include <memory>
#include <vector>
#include <unordered_set>

#include "./transactions.hpp"
#include "./order_type.hpp"
#include "./order_status.hpp"
#include "./order_orderBook.hpp"

class LimitOrder {
private:
	std::shared_ptr<Order> limit_order_head;
	std::shared_ptr<Order> limit_order_tail;
	int limit_order_length;

public:

	LimitOrder() : limit_order_head(nullptr), limit_order_tail(nullptr), limit_order_length(0) {};

	const std::shared_ptr<Order>& getHead();
	void setHead(const std::shared_ptr<Order>& incoming_order);

	const std::shared_ptr<Order>& getTail();
	void setTail(const std::shared_ptr<Order>& incoming_order);

	int getListLength() const;

	int updateListLength(const int& update_increment);
	void addOrder(Order& incoming_order);
	void deleteOrder(const std::shared_ptr<Order>& order_to_delete);
	void matchOrder(Order& incoming_order, std::unordered_set<unsigned int>& order_set, std::ostringstream& executed_orders);

};

#endif 