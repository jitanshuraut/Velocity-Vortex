#include "Orderbook/transactions.hpp"
#include <vector>
#include <string>
using std::string;
using std::vector;



size_t Transactions::getLength() const {
	return raw_orders.size();
}

size_t Transactions::getNumberOfExecutedOrders() const {
	return getLength() / 2;
}

void Transactions::addOrder(const std::string& raw_order) {
	raw_orders.emplace_back(raw_order);
}