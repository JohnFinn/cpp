cmake_minimum_required(VERSION 3.28)
project(vertex-cover)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(vertex-cover main.cpp)

find_package(bshoshany-thread-pool)
find_package(structopt)
find_package(svector CONFIG REQUIRED)

target_link_libraries(
  vertex-cover
  PRIVATE svector::svector bshoshany-thread-pool::bshoshany-thread-pool
          structopt::structopt)

find_package(GTest REQUIRED)
add_executable(test test.cpp)
target_link_libraries(
  test
  PRIVATE GTest::gtest GTest::gmock svector::svector
  PUBLIC GTest::gtest_main)

find_package(benchmark REQUIRED)
add_executable(bench bench.cpp)
target_link_libraries(bench benchmark::benchmark svector::svector)
