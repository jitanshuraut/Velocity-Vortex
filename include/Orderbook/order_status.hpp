#ifndef ORDER_STATUS_H_
#define ORDER_STATUS_H_


#include <unordered_map>
#include <string>

enum class OrderStatus {
	NEW_ORDER,
	PARTIALLY_FILLED,
	FULFILLED,
	STORED
};

extern std::unordered_map<OrderStatus, std::string> order_status_map;

#endif 