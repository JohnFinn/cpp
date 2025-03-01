#include "graph.hpp"

#include <benchmark/benchmark.h>

namespace {

template <std::ranges::range R> auto vec_from_range(R&& r) {
  auto r_common = r | std::views::common;
  return std::vector<std::ranges::range_value_t<R>>(r_common.begin(),
                                                    r_common.end());
}

std::vector<std::pair<int, int>> rand_input(int seed, int max) {
  std::srand(seed);
  return vec_from_range(
      std::views::iota(0, max) | std::views::transform([max](auto) {
        return std::pair<int, int>(std::rand() % max, std::rand() % max);
      }));
}

} // namespace

static void BM_GraphConstructor(benchmark::State& state) {
  const auto max = state.range(0);
  const auto vec = rand_input(0xb0063f72 + max, max);
  for (auto _ : state) {
    Graph g(vec);
  }
}
BENCHMARK(BM_GraphConstructor)->Range(8, 8 << 10);

static void BM_MinVertexCover(benchmark::State& state) {
  const auto max = state.range(0);
  const auto vec = rand_input(0xb0063f72 + max, max);
  Graph g(vec);
  for (auto _ : state) {
    g.vertex_cover();
  }
}
BENCHMARK(BM_MinVertexCover)->Arg(10)->Arg(20)->Arg(30)->Arg(40)->Arg(50);

BENCHMARK_MAIN();
