#pragma once
#include "Common.hpp"
#include "Order.hpp"
#include <map>
#include <unordered_map>

struct PriceBucket {
    Order* head{nullptr};
    Order* tail{nullptr};

    void append(Order* order);
    void remove(Order* order);
};

class OrderBook {
private:
    // Sorted map for price levels: Bids (descending), Asks (ascending)
    std::map<Price, PriceBucket, std::greater<Price>> bids;
    std::map<Price, PriceBucket, std::less<Price>> asks;

    // Fast lookup for order modifications or cancellations
    std::unordered_map<OrderID, Order*> orderMap;

    // Fixed-size memory pool to prevent runtime allocation leaks
    static constexpr size_t MAX_ORDERS = 100000;
    Order orderPool[MAX_ORDERS];
    Order* freeListHead{nullptr};

    Order* allocateOrder(OrderID id, Price price, Quantity qty, Side side);
    void freeOrder(Order* order);

public:
    OrderBook();
    ~OrderBook() = default;

    void processLimitOrder(OrderID id, Price price, Quantity qty, Side side);
    void cancelOrder(OrderID id);
};
