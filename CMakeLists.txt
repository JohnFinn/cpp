cmake_minimum_required(VERSION 3.28)
project(hft)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)

add_executable(hft main.cpp
        msg_stream_iterator.h
        messages.h)

find_package(GTest REQUIRED)
add_executable(test test.cpp)
target_link_libraries(test PRIVATE GTest::gtest PUBLIC GTest::gtest_main)
