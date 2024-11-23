#include "Executor-Manager-Core/order.hpp"


Order::Order(const std::string& id, const std::string& client_order_id, const std::string& created_at, 
             const std::string& updated_at, const std::string& submitted_at, const std::string& filled_at, 
             const std::string& expired_at, const std::string& canceled_at, const std::string& failed_at, 
             const std::string& replaced_at, const std::string& asset_id, const std::string& symbol, 
             const std::string& asset_class, const std::string& qty, const std::string& filled_qty, 
             const std::string& filled_avg_price, const std::string& order_class, const std::string& order_type, 
             const std::string& side, const std::string& position_intent, const std::string& time_in_force, 
             const std::string& limit_price, const std::string& stop_price, const std::string& trail_price, 
             const std::string& trail_percent, bool extended_hours)
    : id(id), client_order_id(client_order_id), created_at(created_at), updated_at(updated_at),
      submitted_at(submitted_at), filled_at(filled_at), expired_at(expired_at), canceled_at(canceled_at),
      failed_at(failed_at), replaced_at(replaced_at), asset_id(asset_id), symbol(symbol), 
      asset_class(asset_class), qty(qty), filled_qty(filled_qty), filled_avg_price(filled_avg_price),
      order_class(order_class), order_type(order_type), side(side), position_intent(position_intent),
      time_in_force(time_in_force), limit_price(limit_price), stop_price(stop_price),
      trail_price(trail_price), trail_percent(trail_percent), extended_hours(extended_hours) {}

// Getter and setter implementations
std::string Order::getId() const { return id; }
std::string Order::getClientOrderId() const { return client_order_id; }
std::string Order::getCreatedAt() const { return created_at; }
std::string Order::getUpdatedAt() const { return updated_at; }
std::string Order::getSubmittedAt() const { return submitted_at; }
std::string Order::getFilledAt() const { return filled_at; }
std::string Order::getExpiredAt() const { return expired_at; }
std::string Order::getCanceledAt() const { return canceled_at; }
std::string Order::getFailedAt() const { return failed_at; }
std::string Order::getReplacedAt() const { return replaced_at; }
std::string Order::getAssetId() const { return asset_id; }
std::string Order::getSymbol() const { return symbol; }
std::string Order::getAssetClass() const { return asset_class; }
std::string Order::getQty() const { return qty; }
std::string Order::getFilledQty() const { return filled_qty; }
std::string Order::getFilledAvgPrice() const { return filled_avg_price; }
std::string Order::getOrderClass() const { return order_class; }
std::string Order::getOrderType() const { return order_type; }
std::string Order::getSide() const { return side; }
std::string Order::getPositionIntent() const { return position_intent; }
std::string Order::getTimeInForce() const { return time_in_force; }
std::string Order::getLimitPrice() const { return limit_price; }
std::string Order::getStopPrice() const { return stop_price; }
std::string Order::getTrailPrice() const { return trail_price; }
std::string Order::getTrailPercent() const { return trail_percent; }
bool Order::isExtendedHours() const { return extended_hours; }

void Order::setId(const std::string& id) { this->id = id; }
void Order::setClientOrderId(const std::string& client_order_id) { this->client_order_id = client_order_id; }
void Order::setCreatedAt(const std::string& created_at) { this->created_at = created_at; }
void Order::setUpdatedAt(const std::string& updated_at) { this->updated_at = updated_at; }
void Order::setSubmittedAt(const std::string& submitted_at) { this->submitted_at = submitted_at; }
void Order::setFilledAt(const std::string& filled_at) { this->filled_at = filled_at; }
void Order::setExpiredAt(const std::string& expired_at) { this->expired_at = expired_at; }
void Order::setCanceledAt(const std::string& canceled_at) { this->canceled_at = canceled_at; }
void Order::setFailedAt(const std::string& failed_at) { this->failed_at = failed_at; }
void Order::setReplacedAt(const std::string& replaced_at) { this->replaced_at = replaced_at; }
void Order::setAssetId(const std::string& asset_id) { this->asset_id = asset_id; }
void Order::setSymbol(const std::string& symbol) { this->symbol = symbol; }
void Order::setAssetClass(const std::string& asset_class) { this->asset_class = asset_class; }
void Order::setQty(const std::string& qty) { this->qty = qty; }
void Order::setFilledQty(const std::string& filled_qty) { this->filled_qty = filled_qty; }
void Order::setFilledAvgPrice(const std::string& filled_avg_price) { this->filled_avg_price = filled_avg_price; }
void Order::setOrderClass(const std::string& order_class) { this->order_class = order_class; }
void Order::setOrderType(const std::string& order_type) { this->order_type = order_type; }
void Order::setSide(const std::string& side) { this->side = side; }
void Order::setPositionIntent(const std::string& position_intent) { this->position_intent = position_intent; }
void Order::setTimeInForce(const std::string& time_in_force) { this->time_in_force = time_in_force; }
void Order::setLimitPrice(const std::string& limit_price) { this->limit_price = limit_price; }
void Order::setStopPrice(const std::string& stop_price) { this->stop_price = stop_price; }
void Order::setTrailPrice(const std::string& trail_price) { this->trail_price = trail_price; }
void Order::setTrailPercent(const std::string& trail_percent) { this->trail_percent = trail_percent; }
void Order::setExtendedHours(bool extended_hours) { this->extended_hours = extended_hours; }

void Order::display() const {
    std::cout << "Order ID: " << id << std::endl;
    std::cout << "Client Order ID: " << client_order_id << std::endl;
    std::cout << "Created At: " << created_at << std::endl;
    std::cout << "Updated At: " << updated_at << std::endl;
    std::cout << "Submitted At: " << submitted_at << std::endl;
    std::cout << "Filled At: " << filled_at << std::endl;
    std::cout << "Expired At: " << expired_at << std::endl;
    std::cout << "Canceled At: " << canceled_at << std::endl;
    std::cout << "Failed At: " << failed_at << std::endl;
    std::cout << "Replaced At: " << replaced_at << std::endl;
    std::cout << "Asset ID: " << asset_id << std::endl;
    std::cout << "Symbol: " << symbol << std::endl;
    std::cout << "Asset Class: " << asset_class << std::endl;
    std::cout << "Quantity: " << qty << std::endl;
    std::cout << "Filled Quantity: " << filled_qty << std::endl;
    std::cout << "Filled Average Price: " << filled_avg_price << std::endl;
    std::cout << "Order Class: " << order_class << std::endl;
    std::cout << "Order Type: " << order_type << std::endl;
    std::cout << "Side: " << side << std::endl;
    std::cout << "Position Intent: " << position_intent << std::endl;
    std::cout << "Time In Force: " << time_in_force << std::endl;
    std::cout << "Limit Price: " << limit_price << std::endl;
    std::cout << "Stop Price: " << stop_price << std::endl;
    std::cout << "Trail Price: " << trail_price << std::endl;
    std::cout << "Trail Percent: " << trail_percent << std::endl;
    std::cout << "Extended Hours: " << (extended_hours ? "Yes" : "No") << std::endl;
}