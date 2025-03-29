#include "graph.hpp"
#include "util.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

void PrintTo(const Graph& g, ::std::ostream *os) {
  auto edges = g.edges();
  std::set<Graph::Edge> sorted(edges.begin(), edges.end());
  ::testing::internal::UniversalPrinter<std::set<Graph::Edge>>::Print(sorted,
                                                                      os);
}

Graph makeGraph(std::initializer_list<Graph::Edge> edges) {
  return Graph(edges);
}

TEST(foo, bar) {
  using namespace ::testing;
  EXPECT_THAT(makeGraph({}).vertex_cover().second, UnorderedElementsAre());
  EXPECT_THAT(makeGraph({{1, 1}}).vertex_cover().second,
              UnorderedElementsAre(1));
  EXPECT_THAT(makeGraph({{1, 1}, {2, 2}}).vertex_cover().second,
              UnorderedElementsAre(1, 2));
  EXPECT_THAT(makeGraph({{1, 2}}).vertex_cover().second,
              UnorderedElementsAre(AnyOf(1, 2)));
  EXPECT_THAT(makeGraph({{2, 1}, {3, 4}}).vertex_cover().second,
              UnorderedElementsAre(AnyOf(1, 2), AnyOf(3, 4)));
  EXPECT_THAT(makeGraph({{1, 2}, {2, 3}, {3, 1}}).vertex_cover().second,
              Not(UnorderedElementsAre(1, 2, 3)));
  EXPECT_THAT(makeGraph({{1, 2}, {2, 3}, {3, 4}, {4, 1}}).vertex_cover().second,
              SizeIs(2));
  EXPECT_THAT(
      makeGraph({{1, 2}, {2, 3}, {3, 4}, {4, 5}, {1, 5}}).vertex_cover().second,
      SizeIs(3));
  EXPECT_THAT(makeGraph({{1, 2}, {2, 3}, {3, 4}, {4, 5}, {1, 5}, {1, 3}})
                  .vertex_cover()
                  .second,
              AllOf(SizeIs(3), Contains(AnyOf(1, 3))));
  EXPECT_THAT(makeGraph({{1, 2}, {2, 3}, {4, 5}, {4, 6}}).vertex_cover().second,
              UnorderedElementsAre(2, 4));
  EXPECT_THAT(makeGraph({{1, 2}, {1, 3}, {1, 4}, {1, 5}}).vertex_cover().second,
              UnorderedElementsAre(1));
  EXPECT_THAT(makeGraph({{2, 1}, {1, 3}, {4, 1}, {1, 5}}).vertex_cover().second,
              UnorderedElementsAre(1));
  EXPECT_THAT(makeGraph({{1, 2},
                         {1, 3},
                         {1, 4},
                         {1, 5},
                         {10, 20},
                         {10, 30},
                         {10, 40},
                         {10, 50}})
                  .vertex_cover()
                  .second,
              UnorderedElementsAre(1, 10));
}

TEST(foo, scc) {
  using namespace ::testing;
  EXPECT_THAT(makeGraph({}).split_into_scc(), SizeIs(0));
  EXPECT_THAT(makeGraph({{1, 1}}).split_into_scc(),
              UnorderedElementsAre(Eq(makeGraph({{1, 1}}))));
  EXPECT_THAT(makeGraph({{1, 1}, {2, 2}}).split_into_scc(),
              UnorderedElementsAre(makeGraph({{1, 1}}), makeGraph({{2, 2}})));
  EXPECT_THAT(makeGraph({{1, 2}}).split_into_scc(),
              UnorderedElementsAre(makeGraph({{1, 2}})));
  EXPECT_THAT(makeGraph({{2, 1}, {3, 4}}).split_into_scc(),
              UnorderedElementsAre(makeGraph({{2, 1}}), makeGraph({{3, 4}})));
  EXPECT_THAT(makeGraph({{1, 2}, {2, 3}, {3, 1}}).split_into_scc(),
              UnorderedElementsAre(makeGraph({{1, 2}, {2, 3}, {3, 1}})));
  EXPECT_THAT(
      makeGraph({{1, 2}, {2, 3}, {3, 4}, {4, 1}}).split_into_scc(),
      UnorderedElementsAre(makeGraph({{1, 2}, {2, 3}, {3, 4}, {4, 1}})));
  EXPECT_THAT(makeGraph({{1, 2},
                         {1, 3},
                         {1, 4},
                         {1, 5},
                         {10, 20},
                         {10, 30},
                         {10, 40},
                         {10, 50}})
                  .split_into_scc(),
              UnorderedElementsAre(
                  makeGraph({{1, 2}, {1, 3}, {1, 4}, {1, 5}}),
                  makeGraph({{10, 20}, {10, 30}, {10, 40}, {10, 50}})));
  EXPECT_THAT(
      makeGraph({{1, 2},
                 {1, 3},
                 {2, 3},
                 {10, 20},
                 {10, 30},
                 {10, 40},
                 {10, 50},
                 {100, 200}})
          .split_into_scc(),
      UnorderedElementsAre(makeGraph({{1, 2}, {1, 3}, {2, 3}}),
                           makeGraph({{10, 20}, {10, 30}, {10, 40}, {10, 50}}),
                           makeGraph({{100, 200}})));
}

TEST(util, pipe) {
  using namespace ::testing;
  pipe_fds p;
  p.serialize(std::optional<float>(3.14));
  ASSERT_TRUE(p.poll_read(std::chrono::milliseconds(0)));
  EXPECT_EQ(p.deserialize<std::optional<float>>(), std::optional<float>(3.14));
}

TEST(util, timeout) {
  EXPECT_EQ(timeout(std::chrono::seconds(1), [] { return 3.14; }),
            std::optional(3.14));
  EXPECT_EQ(timeout(std::chrono::seconds(1),
                    [] {
                      std::this_thread::sleep_for(std::chrono::seconds(2));
                      return 3.14;
                    }),
            std::nullopt);
}
