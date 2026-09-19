#pragma once
#include "Common.hpp"

struct alignas(64) Order {
    OrderID id;
    Price price;
    Quantity quantity;
    Side side;

    // Pointers for an intrusive, zero-allocation doubly-linked list!
    Order* next{nullptr};
    Order* prev{nullptr};
};
