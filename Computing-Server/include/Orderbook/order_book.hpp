#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include "./order_.hpp"
#include "./transactions.hpp"
#include <map>
#include <unordered_map>
#include <list>
#include <vector>
#include <mutex>
struct LevelInfo
{
  Price price_;
  Quantity quantity_;
};
using LevelInfos = std::vector<LevelInfo>;

class OrderbookLevelInfos
{
public:
  OrderbookLevelInfos(const LevelInfos &bids, const LevelInfos &asks);

  /**
   * @brief Gets the bid levels information.
   * @return Bid levels information.
   */
  const LevelInfos &GetBids() const;

  /**
   * @brief Gets the ask levels information.
   * @return Ask levels information.
   */
  const LevelInfos &GetAsks() const;

private:
  LevelInfos bids_;
  LevelInfos asks_;
};

class Orderbook
{
public:
  Trades_ AddOrder(OrderPointer order);
  void CancelOrder(OrderId orderId);
  Trades_ ModifyOrder(OrderPointer order);
  std::size_t Size() const;
  OrderbookLevelInfos GetOrderInfos() const;
  bool CanMatch(Side side, Price price) const;
  std::pair<std::string, std::string> executeMarketMakingStrategy(int CounterId, double bid_price, double ask_price, Quantity bid_quantity, Quantity ask_quantity);
  void setSpreadThreshold(double threshold);
  void setImbalanceThreshold(double threshold);
  void setPositionLimit(int limit);
  void setMaxpositionsize(int siz);
  void setMaxTradeSize(int max_trade_size);
  int getCurrentPosition() const;

private:
  struct OrderEntry
  {
    OrderPointer order_;
    std::list<OrderPointer>::iterator location_;
  };

  std::map<Price, std::list<OrderPointer>, std::greater<Price>> bids_;
  std::map<Price, std::list<OrderPointer>, std::less<Price>> asks_;
  std::unordered_map<OrderId, OrderEntry> orders_;
  double spread_threshold_{0.02};
  double imbalance_threshold_{0.65};
  int position_limit_{1000};
  int current_position_{0};
  int max_position_size_{0};
  int max_trade_size_{0};

  std::vector<Trade_> last_trades_;
  std::map<OrderId, double> entry_prices_;
  mutable std::mutex orderbook_mutex;
  Trades_ MatchOrders();
};

#endif