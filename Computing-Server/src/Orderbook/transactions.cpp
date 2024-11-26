#include "Orderbook/transactions.hpp"

Trade_::Trade_(const TradeInfo& bidTrade, const TradeInfo& askTrade)
    : bidTrade_{bidTrade}, askTrade_{askTrade} {}

const TradeInfo& Trade_::GetBidTrade() const { return bidTrade_; }
const TradeInfo& Trade_::GetAskTrade() const { return askTrade_; }