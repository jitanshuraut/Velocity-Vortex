#include "Orderbook/order_.hpp"

Order_::Order_(OrderType orderType, OrderId orderId, Side side, Price price, Quantity quantity)
	: orderType_{orderType}, orderId_{orderId}, side_{side}, price_{price}, initialQuantity_{quantity}, remainingQuantity_{quantity} {}

OrderId Order_::GetOrderId() const { return orderId_; }
OrderType Order_::GetOrderType() const { return orderType_; }
Side Order_::GetSide() const { return side_; }
Price Order_::GetPrice() const { return price_; }
Quantity Order_::GetInitialQuantity() const { return initialQuantity_; }
Quantity Order_::GetRemainingQuantity() const { return remainingQuantity_; }
Quantity Order_::GetFilledQuantity() const { return initialQuantity_ - remainingQuantity_; }
bool Order_::IsFilled() const { return GetRemainingQuantity() == 0; }

void Order_::Fill(Quantity quantity)
{
	if (quantity > GetRemainingQuantity())
	{

		return;
	}
	remainingQuantity_ -= quantity;
}