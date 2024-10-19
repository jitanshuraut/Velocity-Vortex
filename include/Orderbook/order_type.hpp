#ifndef ORDER_TYPE_H_
#define ORDER_TYPE_H_


#include <unordered_map>
#include <string>

enum class OrderType {
	BUY,
	SELL,
	CANCEL
};

extern std::unordered_map<OrderType, std::string> order_type_map;

#endif 