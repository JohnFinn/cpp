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

static void BM_MinVertexCover(benchmark::State& state) {
  const auto max = state.range(0);
  std::vector<std::pair<int, int>> vec(max);
  std::generate(vec.begin(), vec.end(), [max]() {
    return std::pair(std::rand() % max, std::rand() % max);
  });
  Graph g(vec);
  for (auto _ : state) {
    g.vertex_cover();
  }
}
BENCHMARK(BM_MinVertexCover)->Arg(10)->Arg(20)->Arg(30)->Arg(40)->Arg(50);

BENCHMARK_MAIN();
