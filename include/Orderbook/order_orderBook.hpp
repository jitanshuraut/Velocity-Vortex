#ifndef ORDER_H_
#define ORDER_H_

#pragma once
#include <string>
#include <ctime>
#include <chrono>
#include <memory>
#include <unordered_map>
#include <iostream>
#include "./order_type.hpp"
#include "./order_status.hpp"

class Order
{
public:
  std::string symbol;
  unsigned int id;

  OrderType type;

  float price;
  float original_quantity;

  OrderStatus order_status;

  float peak_quantity;
  float quantity_transacted;
  float total_quantity_transacted;

  std::shared_ptr<Order> next_order;
  std::shared_ptr<Order> prev_order;

  Order(std::string input_symbol, unsigned int input_id, OrderType input_type, float input_price, float input_original_quantity);

  const std::string &getSymbol();
  const unsigned int &getID() const;
  const OrderType &getType() const;
  const float &getPrice() const;
  const OrderStatus &getStatus() const;
  const float &getOriginalQuantity() const;
  const float &getQuantityTransacted() const;
  const float &getPeakQuantity() const;
  const float &getTotalQuantityTransacted() const;

  void setOrderStatus(const OrderStatus &new_status);
  void setTotalQuantityTransacted(const float &input_quantity);
  void setQuantityTransacted(const float &input_quantity);
  void setPeakQuantity(const float &input_quantity);

  bool Match(Order &incoming_oder);

  void executeTransaction(const float &input_transaction_quantity);

  void storeOrderInfo(std::ostringstream &output_data, const std::string &input_color);
};

#endif