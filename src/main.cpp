#include "OrderBook.hpp"
#include <chrono>
#include <iostream>

int main() {
    OrderBook book;
    std::cout << "Warming up caches and starting benchmark..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    // Run high performance trading simulator benchmark loops
    for (uint64_t i = 1; i <= 50000; ++i) {
        book.processLimitOrder(i, 1000 + (i % 10), 100, Side::Buy);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    std::cout << "Processed 50,000 orders." << std::endl;
    std::cout << "Total time: " << duration << " ns" << std::endl;
    std::cout << "Average latency per order: " << (duration / 50000) << " ns" << std::endl;

    return 0;
}
