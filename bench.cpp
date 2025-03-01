#include "graph.hpp"

#include <benchmark/benchmark.h>

static void BM_GraphConstructor(benchmark::State& state) {
  const auto max = state.range(0);
  std::vector<std::pair<int, int>> vec(max);
  std::generate(vec.begin(), vec.end(), [max]() {
    return std::pair(std::rand() % max, std::rand() % max);
  });
  for (auto _ : state) {
    Graph g(vec);
  }
}
BENCHMARK(BM_GraphConstructor)->Range(8, 8 << 10);

BENCHMARK_MAIN();
