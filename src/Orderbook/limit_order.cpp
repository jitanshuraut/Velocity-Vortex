#include "Orderbook/limit_order.hpp"
#include <memory>
#include <sstream>
#include <unordered_set>
#include "Orderbook/transactions.hpp" 
#include "Orderbook/order_type.hpp"
#include "Orderbook/order_status.hpp"
#include "Orderbook/order_orderBook.hpp"
using std::shared_ptr;
using std::unordered_set;
using std::ostringstream;

const shared_ptr<Order>& LimitOrder::getHead() {
	return limit_order_head;
}

void LimitOrder::setHead(const std::shared_ptr<Order>& incoming_order) {
	limit_order_head = incoming_order;
}

const shared_ptr<Order>& LimitOrder::getTail() {
	return limit_order_tail;
}

void LimitOrder::setTail(const std::shared_ptr<Order>& incoming_order) {
	limit_order_tail = incoming_order;
}

int LimitOrder::getListLength() const {
	return limit_order_length;
}

int LimitOrder::updateListLength(const int& update_increment) {
	return limit_order_length += update_increment;
}

void LimitOrder::addOrder(Order& incoming_order) {

	const std::shared_ptr<Order>& incoming_order_ptr = std::make_shared<Order>(incoming_order);
	limit_order_length++;

	if (limit_order_length == 1) {
		limit_order_head = incoming_order_ptr;
		limit_order_tail = incoming_order_ptr;
	}

	else {
		incoming_order_ptr->prev_order = limit_order_tail;
		limit_order_tail->next_order = incoming_order_ptr;
		limit_order_tail = incoming_order_ptr;
	}

}

void LimitOrder::deleteOrder(const std::shared_ptr<Order>& order_to_delete) {


	limit_order_length--;

	if (limit_order_length - 1 <= 0) {
		limit_order_head = nullptr;
		limit_order_tail = nullptr;
		return;
	}

	const shared_ptr<Order>& nxt_order = order_to_delete->next_order;
	const shared_ptr<Order>& prv_order = order_to_delete->prev_order;

	if (nxt_order != nullptr && prv_order != nullptr) {
		nxt_order->prev_order = prv_order;
		prv_order->next_order = nxt_order;
	}


	else if (nxt_order != nullptr) {
		nxt_order->prev_order = nullptr;
		limit_order_head = nxt_order;
	}

	/* Delete the tail */
	else if (prv_order != nullptr) {
		prv_order->next_order = nullptr;
		limit_order_tail = prv_order; 
	}

}

void LimitOrder::matchOrder(Order& incoming_order, unordered_set<unsigned int>& order_set, ostringstream& executed_orders) {
	shared_ptr<Order> current_linked_list_order = limit_order_head;

	while (incoming_order.getPeakQuantity() > 0 && limit_order_length > 0) {

		bool matched_order = current_linked_list_order->Match(incoming_order);

		if (matched_order) {

			shared_ptr<Order> next_current_linked_list_order = current_linked_list_order->next_order;
			current_linked_list_order->storeOrderInfo(executed_orders, "\033[32m");

			order_set.erase(current_linked_list_order->getID());
			deleteOrder(current_linked_list_order);
			current_linked_list_order.reset();
			current_linked_list_order = next_current_linked_list_order;
		}
	}
	return;
}