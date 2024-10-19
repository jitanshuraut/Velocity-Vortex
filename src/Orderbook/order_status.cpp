#include "Orderbook/order_status.hpp"
#include <string>
#include <unordered_map>
using std::string;
using std::unordered_map;

unordered_map<OrderStatus, string> order_status_map = {
	{OrderStatus::NEW_ORDER, "NEW ORDER"},
	{OrderStatus::PARTIALLY_FILLED, "PARTIALLY FILLED"},
	{OrderStatus::FULFILLED, "FULFILLED"},
	{OrderStatus::STORED, "STORED"}};