#include "Orderbook/order_orderBook.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <memory>
#include <string>
#include "Orderbook/order_type.hpp"
#include "Orderbook/order_status.hpp"

using std::cout;
using std::endl;
using std::min;
using std::ostringstream;
using std::shared_ptr;
using std::string;
using std::to_string;

Order::Order(string input_symbol, unsigned int input_id, OrderType input_type, float input_price, float input_original_quantity) : symbol(input_symbol), id(input_id), type(input_type), price(input_price), original_quantity(input_original_quantity)
{

	order_status = OrderStatus::NEW_ORDER;

	peak_quantity = input_original_quantity;
	quantity_transacted = 0;
	total_quantity_transacted = 0;

	next_order = nullptr;
	prev_order = nullptr;
}

const std::string &Order::getSymbol()
{
	return symbol;
}

const unsigned int &Order::getID() const
{
	return id;
}

const OrderType &Order::getType() const
{
	return type;
}

const float &Order::getPrice() const
{
	return price;
}

const OrderStatus &Order::getStatus() const
{
	return order_status;
}

const float &Order::getOriginalQuantity() const
{
	return original_quantity;
}

const float &Order::getQuantityTransacted() const
{
	return quantity_transacted;
}

const float &Order::getPeakQuantity() const
{
	return peak_quantity;
}

const float &Order::getTotalQuantityTransacted() const
{
	return total_quantity_transacted;
}

void Order::setOrderStatus(const OrderStatus &new_status)
{
	order_status = new_status;
}

void Order::setTotalQuantityTransacted(const float &input_quantity)
{
	total_quantity_transacted += input_quantity;
}

void Order::setQuantityTransacted(const float &input_quantity)
{
	quantity_transacted = input_quantity;
}

void Order::setPeakQuantity(const float &input_quantity)
{
	peak_quantity += input_quantity;
}

bool Order::Match(Order &incoming_order)
{

	if (peak_quantity <= incoming_order.getPeakQuantity())
	{

		order_status = OrderStatus::FULFILLED;
		if (peak_quantity == incoming_order.getPeakQuantity())
		{
			incoming_order.setOrderStatus(OrderStatus::FULFILLED);
		}
		else
		{
			incoming_order.setOrderStatus(OrderStatus::PARTIALLY_FILLED);
		}

		auto transaction_quantity = peak_quantity;
		this->executeTransaction(transaction_quantity);
		incoming_order.executeTransaction(transaction_quantity);

		return true;
	}

	else
	{

		order_status = OrderStatus::PARTIALLY_FILLED;
		incoming_order.setOrderStatus(OrderStatus::FULFILLED);

		auto transaction_quantity = incoming_order.peak_quantity;
		this->executeTransaction(transaction_quantity);
		incoming_order.executeTransaction(transaction_quantity);

		return false;
	}
}

void Order::executeTransaction(const float &input_transaction_quantity)
{

	setTotalQuantityTransacted(input_transaction_quantity);
	setQuantityTransacted(input_transaction_quantity);
	setPeakQuantity(-input_transaction_quantity);
}

void Order::storeOrderInfo(ostringstream &output_data, const string &input_color)
{

	output_data << input_color << "Order:- Symbol: " << symbol << ", ID: " << id << ", Type: " << order_type_map[type] << ", Price: " << price << ", Orignal Quantity: " << original_quantity << ", Quantity Left: " << peak_quantity << ", Status: " << order_status_map[order_status] << "\033[0m" << std::endl;
}