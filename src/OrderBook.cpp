#include "OrderBook.hpp"
#include <stdexcept>
#include <algorithm>

void PriceBucket::append(Order* order) {
    if (!head) {
        head = tail = order;
    } else {
        tail->next = order;
        order->prev = tail;
        tail = order;
    }
}

void PriceBucket::remove(Order* order) {
    if (order->prev) order->prev->next = order->next;
    if (order->next) order->next->prev = order->prev;
    if (order == head) head = order->next;
    if (order == tail) tail = order->prev;
    order->next = nullptr;
    order->prev = nullptr;
}

OrderBook::OrderBook() {
    for (size_t i = 0; i < MAX_ORDERS - 1; ++i) {
        orderPool[i].next = &orderPool[i + 1];
    }
    orderPool[MAX_ORDERS - 1].next = nullptr;
  freeListHead = orderPool; //  This fixes the type conversion mismatch

}

Order* OrderBook::allocateOrder(OrderID id, Price price, Quantity qty, Side side) {
    if (!freeListHead) {
        throw std::runtime_error("Order pool exhausted!");
    }
    Order* order = freeListHead;
    freeListHead = freeListHead->next;

    order->id = id;
    order->price = price;
    order->quantity = qty;
    order->side = side;
    order->next = nullptr;
    order->prev = nullptr;
    return order;
}

void OrderBook::freeOrder(Order* order) {
    order->next = freeListHead;
    freeListHead = order;
}

void OrderBook::processLimitOrder(OrderID id, Price price, Quantity qty, Side side) {
    Quantity remainingQty = qty;

    if (side == Side::Buy) {
        auto it = asks.begin();
        while (it != asks.end() && it->first <= price && remainingQty > 0) {
            PriceBucket& bucket = it->second;
            Order* makerOrder = bucket.head;

            while (makerOrder && remainingQty > 0) {
                Order* nextMaker = makerOrder->next;
                
                // Manual ternary operator avoids std::min template errors on old g++ versions
                Quantity makerQty = makerOrder->quantity;
                Quantity matchQty = (remainingQty < makerQty) ? remainingQty : makerQty;

                remainingQty -= matchQty;
                makerOrder->quantity -= matchQty;

                if (makerOrder->quantity == 0) {
                    bucket.remove(makerOrder);
                    orderMap.erase(makerOrder->id);
                    freeOrder(makerOrder);
                }
                makerOrder = nextMaker;
            }

            if (!bucket.head) {
                asks.erase(it++);
            } else {
                ++it;
            }
        }

        if (remainingQty > 0) {
            Order* newOrder = allocateOrder(id, price, remainingQty, side);
            orderMap[id] = newOrder;
            bids[price].append(newOrder);
        }
    } else {
        auto it = bids.begin();
        while (it != bids.end() && it->first >= price && remainingQty > 0) {
            PriceBucket& bucket = it->second;
            Order* makerOrder = bucket.head;

            while (makerOrder && remainingQty > 0) {
                Order* nextMaker = makerOrder->next;
                
                // Manual ternary operator avoids std::min template errors on old g++ versions
                Quantity makerQty = makerOrder->quantity;
                Quantity matchQty = (remainingQty < makerQty) ? remainingQty : makerQty;

                remainingQty -= matchQty;
                makerOrder->quantity -= matchQty;

                if (makerOrder->quantity == 0) {
                    bucket.remove(makerOrder);
                    orderMap.erase(makerOrder->id);
                    freeOrder(makerOrder);
                }
                makerOrder = nextMaker;
            }

            if (!bucket.head) {
                bids.erase(it++);
            } else {
                ++it;
            }
        }

        if (remainingQty > 0) {
            Order* newOrder = allocateOrder(id, price, remainingQty, side);
            orderMap[id] = newOrder;
            asks[price].append(newOrder);
        }
    }
}

void OrderBook::cancelOrder(OrderID id) {
    auto it = orderMap.find(id);
    if (it == orderMap.end()) return;

    Order* order = it->second;
    if (order->side == Side::Buy) {
        auto bucketIt = bids.find(order->price);
        bucketIt->second.remove(order);
        if (!bucketIt->second.head) {
            bids.erase(bucketIt);
        }
    } else {
        auto bucketIt = asks.find(order->price);
        bucketIt->second.remove(order);
        if (!bucketIt->second.head) {
            asks.erase(bucketIt);
        }
    }

    freeOrder(order);
    orderMap.erase(it);
}
