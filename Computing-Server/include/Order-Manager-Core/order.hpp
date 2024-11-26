#ifndef ORDER_HPP
#define ORDER_HPP

#include <string>
#include <iostream>

class Order {
public:
    // Constructor
    Order(const std::string& id, const std::string& client_order_id, const std::string& created_at, 
          const std::string& updated_at, const std::string& submitted_at, const std::string& filled_at, 
          const std::string& expired_at, const std::string& canceled_at, const std::string& failed_at, 
          const std::string& replaced_at, const std::string& asset_id, const std::string& symbol, 
          const std::string& asset_class, const std::string& qty, const std::string& filled_qty, 
          const std::string& filled_avg_price, const std::string& order_class, const std::string& order_type, 
          const std::string& side, const std::string& position_intent, const std::string& time_in_force, 
          const std::string& limit_price, const std::string& stop_price, const std::string& trail_price, 
          const std::string& trail_percent, bool extended_hours);

    // Getters
    std::string getId() const;
    std::string getClientOrderId() const;
    std::string getCreatedAt() const;
    std::string getUpdatedAt() const;
    std::string getSubmittedAt() const;
    std::string getFilledAt() const;
    std::string getExpiredAt() const;
    std::string getCanceledAt() const;
    std::string getFailedAt() const;
    std::string getReplacedAt() const;
    std::string getAssetId() const;
    std::string getSymbol() const;
    std::string getAssetClass() const;
    std::string getQty() const;
    std::string getFilledQty() const;
    std::string getFilledAvgPrice() const;
    std::string getOrderClass() const;
    std::string getOrderType() const;
    std::string getSide() const; 
    std::string getPositionIntent() const;
    std::string getTimeInForce() const; 
    std::string getLimitPrice() const;
    std::string getStopPrice() const;
    std::string getTrailPrice() const;
    std::string getTrailPercent() const;
    bool isExtendedHours() const;

    // Setters
    void setId(const std::string& id);
    void setClientOrderId(const std::string& client_order_id);
    void setCreatedAt(const std::string& created_at);
    void setUpdatedAt(const std::string& updated_at);
    void setSubmittedAt(const std::string& submitted_at);
    void setFilledAt(const std::string& filled_at);
    void setExpiredAt(const std::string& expired_at);
    void setCanceledAt(const std::string& canceled_at);
    void setFailedAt(const std::string& failed_at);
    void setReplacedAt(const std::string& replaced_at);
    void setAssetId(const std::string& asset_id);
    void setSymbol(const std::string& symbol);
    void setAssetClass(const std::string& asset_class);
    void setQty(const std::string& qty);
    void setFilledQty(const std::string& filled_qty);
    void setFilledAvgPrice(const std::string& filled_avg_price);
    void setOrderClass(const std::string& order_class);
    void setOrderType(const std::string& order_type);
    void setSide(const std::string& side); // "buy" or "sell"
    void setPositionIntent(const std::string& position_intent);
    void setTimeInForce(const std::string& time_in_force); 
    void setLimitPrice(const std::string& limit_price);
    void setStopPrice(const std::string& stop_price);
    void setTrailPrice(const std::string& trail_price);
    void setTrailPercent(const std::string& trail_percent);
    void setExtendedHours(bool extended_hours);


    void display() const;

private:
    
    std::string id;
    std::string client_order_id;
    std::string created_at;
    std::string updated_at;
    std::string submitted_at;
    std::string filled_at;
    std::string expired_at;
    std::string canceled_at;
    std::string failed_at;
    std::string replaced_at;
    std::string asset_id;
    std::string symbol;
    std::string asset_class;
    std::string qty;
    std::string filled_qty;
    std::string filled_avg_price;
    std::string order_class;
    std::string order_type;
    std::string side; // "buy" or "sell"
    std::string position_intent;
    std::string time_in_force; 
    std::string limit_price;
    std::string stop_price;
    std::string trail_price;
    std::string trail_percent;
    bool extended_hours;
};

#endif 