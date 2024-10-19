#ifndef ORDER_BOOK_H_
#define ORDER_BOOK_H_

#include<string>
using std::string;
#include <sstream>
using std::ostringstream;
#include "./transactions.hpp"
#include "./order_type.hpp"
#include "./order_status.hpp"
#include "./order_orderBook.hpp"
#include "./limit_order.hpp"
#include "./order_tree.hpp"

class OrderBook {
private:
  OrderTree buy_tree;
  OrderTree sell_tree;
  ostringstream output_data;
  int total_orders_submitted;

public:
  OrderBook() : buy_tree(OrderType::BUY), sell_tree(OrderType::SELL), total_orders_submitted(0){};
  OrderTree& getBuyTree();
  OrderTree& getSellTree();
  OrderTree& getOrderTreeByOrderType(const OrderType& order_type);
  
  const int& getTotalOrdersSubmitted();
  const int& getTotalOrdersFulfilled();
  const int& getTotalOrdersUnfulfilled();

  string getResults();

  void processOrder(Order& incoming_order);

  void cancelOrder(const std::shared_ptr<Order>& cancel_order);

  void getCurrentSnapshot();

};

#endif 