#pragma once
#include <cstdint>
#include <string> // Changed from <string_view>

enum class Side : uint8_t { Buy, Sell };
enum class OrderType : uint8_t { Limit, Market };

using OrderID = uint64_t;
using Price = uint32_t; 
using Quantity = uint32_t;
