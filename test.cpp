#include "graph.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(foo, bar) {
  EXPECT_THAT(Graph(std::vector<Graph::Edge>{{1, 2}, {2, 3}, {4, 5}, {4, 6}})
                  .vertex_cover(),
              ::testing::UnorderedElementsAre(2, 4));
}
