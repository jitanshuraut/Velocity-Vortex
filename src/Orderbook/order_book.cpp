#include "Orderbook/order_book.hpp"
#include <numeric>

OrderbookLevelInfos::OrderbookLevelInfos(const LevelInfos &bids, const LevelInfos &asks)
    : bids_(bids), asks_(asks) {}

const LevelInfos &OrderbookLevelInfos::GetBids() const { return bids_; }
const LevelInfos &OrderbookLevelInfos::GetAsks() const { return asks_; }

Trades_ Orderbook::AddOrder(OrderPointer order)
{
    std::unique_lock<std::mutex> lock(orderbook_mutex);
    if (orders_.find(order->GetOrderId()) != orders_.end())
    {
        return {};
    }
    if (order->GetOrderType() == OrderType::FillAndKill && !CanMatch(order->GetSide(), order->GetPrice()))
    {
        return {};
    }

    if (order->GetSide() == Side::Buy)
    {
        auto &orders = bids_[order->GetPrice()];
        orders.push_back(order);
        auto iterator = std::prev(orders.end());
        OrderEntry entry{order, iterator};
        orders_.emplace(order->GetOrderId(), entry);
    }
    else
    {
        auto &orders = asks_[order->GetPrice()];
        orders.push_back(order);
        auto iterator = std::prev(orders.end());
        OrderEntry entry{order, iterator};
        orders_.emplace(order->GetOrderId(), entry);
    }

    return MatchOrders();
}

bool Orderbook::CanMatch(Side side, Price price) const
{
    std::lock_guard<std::mutex> lock(orderbook_mutex);

    if (side == Side::Buy)
    {
        return !asks_.empty() && price >= asks_.begin()->first;
    }
    else
    {
        return !bids_.empty() && price <= bids_.begin()->first;
    }
}

Trades_ Orderbook::MatchOrders()
{

    Trades_ trades;
    trades.reserve(orders_.size());

    while (!bids_.empty() && !asks_.empty())
    {
        auto &bid_entry = *bids_.begin();
        auto &ask_entry = *asks_.begin();

        if (bid_entry.first < ask_entry.first)
        {
            break;
        }

        auto &bids = bid_entry.second;
        auto &asks = ask_entry.second;

        while (!bids.empty() && !asks.empty())
        {
            auto &bid_order = bids.front();
            auto &ask_order = asks.front();

            auto trade_quantity = std::min(bid_order->GetRemainingQuantity(),
                                           ask_order->GetRemainingQuantity());

            if (trade_quantity > 0)
            {

                bid_order->Fill(trade_quantity);
                ask_order->Fill(trade_quantity);

                trades.push_back(Trade_{
                    TradeInfo{bid_order->GetOrderId(), trade_quantity, bid_order->GetPrice()},
                    TradeInfo{ask_order->GetOrderId(), trade_quantity, ask_order->GetPrice()}});

                if (bid_order->IsFilled())
                {
                    orders_.erase(bid_order->GetOrderId());
                    bids.pop_front();
                }
                if (ask_order->IsFilled())
                {
                    orders_.erase(ask_order->GetOrderId());
                    asks.pop_front();
                }
            }

            if (bids.empty())
            {
                bids_.erase(bid_entry.first);
                break;
            }
            if (asks.empty())
            {
                asks_.erase(ask_entry.first);
                break;
            }
        }
    }

    auto cancel_fak = [this](auto &orders_map)
    {
        if (!orders_map.empty())
        {
            auto &orders = orders_map.begin()->second;
            if (!orders.empty() && orders.front()->GetOrderType() == OrderType::FillAndKill)
            {
                CancelOrder(orders.front()->GetOrderId());
            }
        }
    };

    cancel_fak(bids_);
    cancel_fak(asks_);

    return trades;
}

void Orderbook::CancelOrder(OrderId orderId)
{
    std::lock_guard<std::mutex> lock(orderbook_mutex);

    auto order_it = orders_.find(orderId);
    if (order_it == orders_.end())
    {
        return;
    }

    const auto &order_entry = order_it->second;
    const auto &order = order_entry.order_;

    if (order->GetSide() == Side::Buy)
    {
        auto &orders = bids_[order->GetPrice()];
        orders.erase(order_entry.location_);
        if (orders.empty())
        {
            bids_.erase(order->GetPrice());
        }
    }
    else
    {
        auto &orders = asks_[order->GetPrice()];
        orders.erase(order_entry.location_);
        if (orders.empty())
        {
            asks_.erase(order->GetPrice());
        }
    }

    orders_.erase(orderId);
}

Trades_ Orderbook::ModifyOrder(OrderPointer order)
{
    std::lock_guard<std::mutex> lock(orderbook_mutex);

    if (orders_.find(order->GetOrderId()) == orders_.end())
    {
        return {};
    }

    CancelOrder(order->GetOrderId());
    return AddOrder(order);
}

std::size_t Orderbook::Size() const
{
    std::lock_guard<std::mutex> lock(orderbook_mutex);
    return orders_.size();
}

OrderbookLevelInfos Orderbook::GetOrderInfos() const
{
    std::lock_guard<std::mutex> lock(orderbook_mutex);

    LevelInfos bid_infos, ask_infos;
    bid_infos.reserve(bids_.size());
    ask_infos.reserve(asks_.size());

    auto create_level_info = [](Price price, const std::list<OrderPointer> &orders)
    {
        Quantity total = std::accumulate(
            orders.begin(), orders.end(), Quantity{0},
            [](Quantity sum, const OrderPointer &order)
            {
                return sum + order->GetRemainingQuantity();
            });
        return LevelInfo{price, total};
    };

    for (const auto &[price, orders] : bids_)
    {
        bid_infos.push_back(create_level_info(price, orders));
    }
    for (const auto &[price, orders] : asks_)
    {
        ask_infos.push_back(create_level_info(price, orders));
    }

    return OrderbookLevelInfos(bid_infos, ask_infos);
}

std::pair<std::string, std::string> Orderbook::executeMarketMakingStrategy(int CounterId, double bid_price, double ask_price, Quantity bid_quantity, Quantity ask_quantity)
{

    {
        std::lock_guard<std::mutex> lock(orderbook_mutex);
        if (current_position_ >= position_limit_ && current_position_ <= -position_limit_)
        {
            return {"hold", ""};
        }
    }

    double spread = ask_price - bid_price;

    if (spread > spread_threshold_)
    {
        if (current_position_ < position_limit_)
        {
            double improved_bid = bid_price + spread_threshold_ / 2;
            OrderId orderId = CounterId;

            auto buy_order = std::make_shared<Order_>(
                OrderType::GoodTillCancel,
                orderId,
                Side::Buy,
                improved_bid,
                std::min(ask_quantity, static_cast<Quantity>(100)));

            auto trades = AddOrder(buy_order);
            {
                std::lock_guard<std::mutex> lock(orderbook_mutex);
                current_position_ += std::min(ask_quantity, static_cast<Quantity>(100));
            }

            return {"buy", std::to_string(improved_bid)};
        }

        if (current_position_ > -position_limit_)
        {
            double improved_ask = ask_price - spread_threshold_ / 2;
            CounterId++;
            OrderId orderId = CounterId;
            auto sell_order = std::make_shared<Order_>(
                OrderType::GoodTillCancel,
                orderId,
                Side::Sell,
                improved_ask,
                std::min(bid_quantity, static_cast<Quantity>(100)));
            auto trades = AddOrder(sell_order);
            {
                std::lock_guard<std::mutex> lock(orderbook_mutex);
                current_position_ -= std::min(bid_quantity, static_cast<Quantity>(100));
            }

            return {"sell", std::to_string(improved_ask)};
        }
    }
    return {"hold", ""};
}

void Orderbook::setSpreadThreshold(double threshold) { spread_threshold_ = threshold; }
void Orderbook::setImbalanceThreshold(double threshold) { imbalance_threshold_ = threshold; }
void Orderbook::setPositionLimit(int limit) { position_limit_ = limit; }
void Orderbook::setMaxpositionsize(int siz) { max_position_size_ = siz; }
void Orderbook::setMaxTradeSize(int max_trade_size) { max_trade_size_ = max_trade_size; }
