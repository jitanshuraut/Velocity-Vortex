#include "Orderbook/order_type.hpp"
#include <string>
#include<unordered_map>
using std::unordered_map;
using std::string;


unordered_map<OrderType, string> order_type_map = {
	{OrderType::BUY,"BUY"},
	{OrderType::SELL,"SELL"},
	{OrderType::CANCEL,"CANCEL"}
};