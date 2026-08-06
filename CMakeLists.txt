cmake_minimum_required(VERSION 3.10)

# Set your project name here
project(CoreMatch LANGUAGES CXX)

# Force the compiler to use modern C++17 properties
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include the 'include' directory so your .cpp files can find the .hpp headers
include_directories(include)

# Gather and connect all your source files into a single executable binary
add_executable(CoreMatch
    src/OrderBook.cpp
    src/main.cpp
)
