#ifndef TRANSACTIONS_H_
#define TRANSACTIONS_H_

#pragma once
#include <string>
#include <vector>

class Transactions
{
private:
	std::vector<std::string> raw_orders;
	std::vector<std::string> json_orders;

public:
	Transactions() : raw_orders({}), json_orders({}) {};

	size_t getLength() const;

	size_t getNumberOfExecutedOrders() const;

	void addOrder(const std::string &raw_order);
};

#endif