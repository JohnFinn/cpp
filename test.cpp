#include "graph.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

Graph makeGraph(std::initializer_list<Graph::Edge> edges) {
  return Graph(std::vector<Graph::Edge>(edges));
}

TEST(foo, bar) {
  using namespace ::testing;
  EXPECT_THAT(makeGraph({}).vertex_cover(), UnorderedElementsAre());
  EXPECT_THAT(makeGraph({{1, 1}}).vertex_cover(), UnorderedElementsAre(1));
  EXPECT_THAT(makeGraph({{1, 1}, {2, 2}}).vertex_cover(),
              UnorderedElementsAre(1, 2));
  EXPECT_THAT(makeGraph({{1, 2}}).vertex_cover(),
              UnorderedElementsAre(AnyOf(1, 2)));
  EXPECT_THAT(makeGraph({{2, 1}, {3, 4}}).vertex_cover(),
              UnorderedElementsAre(AnyOf(1, 2), AnyOf(3, 4)));
  EXPECT_THAT(makeGraph({{1, 2}, {2, 3}, {3, 1}}).vertex_cover(),
              Not(UnorderedElementsAre(1, 2, 3)));
  EXPECT_THAT(makeGraph({{1, 2}, {2, 3}, {3, 4}, {4, 1}}).vertex_cover(),
              SizeIs(2));
  EXPECT_THAT(
      makeGraph({{1, 2}, {2, 3}, {3, 4}, {4, 5}, {1, 5}}).vertex_cover(),
      SizeIs(3));
  EXPECT_THAT(makeGraph({{1, 2}, {2, 3}, {3, 4}, {4, 5}, {1, 5}, {1, 3}})
                  .vertex_cover(),
              AllOf(SizeIs(3), Contains(AnyOf(1, 3))));
  EXPECT_THAT(makeGraph({{1, 2}, {2, 3}, {4, 5}, {4, 6}}).vertex_cover(),
              UnorderedElementsAre(2, 4));
  EXPECT_THAT(makeGraph({{1, 2}, {1, 3}, {1, 4}, {1, 5}}).vertex_cover(),
              UnorderedElementsAre(1));
  EXPECT_THAT(makeGraph({{2, 1}, {1, 3}, {4, 1}, {1, 5}}).vertex_cover(),
              UnorderedElementsAre(1));
  EXPECT_THAT(makeGraph({{1, 2},
                         {1, 3},
                         {1, 4},
                         {1, 5},
                         {10, 20},
                         {10, 30},
                         {10, 40},
                         {10, 50}})
                  .vertex_cover(),
              UnorderedElementsAre(1, 10));
}
