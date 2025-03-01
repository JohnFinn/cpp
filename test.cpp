#include "graph.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(foo, bar) {
  using namespace ::testing;
  EXPECT_THAT(Graph(std::vector<Graph::Edge>{}).vertex_cover(),
              UnorderedElementsAre());
  EXPECT_THAT(Graph(std::vector<Graph::Edge>{{1, 2}, {2, 3}, {4, 5}, {4, 6}})
                  .vertex_cover(),
              UnorderedElementsAre(2, 4));
  EXPECT_THAT(Graph(std::vector<Graph::Edge>{{1, 2}, {1, 3}, {1, 4}, {1, 5}})
                  .vertex_cover(),
              UnorderedElementsAre(1));
  EXPECT_THAT(Graph(std::vector<Graph::Edge>{{2, 1}, {1, 3}, {4, 1}, {1, 5}})
                  .vertex_cover(),
              UnorderedElementsAre(1));
  EXPECT_THAT(Graph(std::vector<Graph::Edge>{{1, 2},
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
