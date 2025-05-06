#include <algorithm>
#include <format>
#include <iostream>
#include <memory>
#include <optional>
#include <ranges>
#include <set>
#include <unordered_map>
#include <vector>

class Graph {
public:
  using Vertex = int;
  using Edge = std::pair<Vertex, Vertex>;

  template <std::ranges::range R>
    requires std::is_same_v<std::ranges::range_value_t<R>, Edge>
  Graph(R&& edges)
      : _adj(std::make_shared<EdgesVector>(edges.begin(), edges.end())),
        _span(_adj->begin(), _adj->end()) {}

  using VertexCover = std::vector<Vertex>;

  using counter_t = long;

  std::pair<counter_t, VertexCover> vertex_cover() const {
    counter_t count = 0;
    for (std::size_t try_vertex_cover_size : std::views::iota(0)) {
#ifdef LOG
      std::cerr << std::format("Trying vertex cover size {}/{}\r",
                               try_vertex_cover_size, _adj->size());
#endif
      if (auto vc = _vc_branch(try_vertex_cover_size, count)) {
        return {count, *vc};
      }
    }
  }

  bool operator==(const Graph& other) const {
    auto to_set = [](const auto& graph) {
      return std::set(graph._span.begin(), graph._span.end());
    };
    return to_set(*this) == to_set(other);
  }

  auto edges() const { return _span; }

private:
  auto _get_unmarked_vertex(std::unordered_map<Vertex, int>& visited) const {
    return std::ranges::find_if(
        visited, [](const auto& pair) { return pair.second == 0; });
  }

public:
  std::vector<Graph> split_into_scc() const {
    std::vector<Graph> result;
    std::unordered_map<Vertex, int> visited;
    for (const auto& [from, to] : _span) {
      visited[from] = 0;
      visited[to] = 0;
    }
    auto the_rest = _span;
    for (auto mark_as : std::views::iota(1)) {
      if (auto vertex = _get_unmarked_vertex(visited);
          vertex != visited.end()) {
        _dfs(visited, vertex->first, mark_as);
        auto r = std::ranges::partition(
            the_rest, [&visited, mark_as](const auto& pair) {
              return visited[pair.first] == mark_as ||
                     visited[pair.second] == mark_as;
            });
        result.emplace_back(Graph(*this));
        result.back()._span = std::span(the_rest.begin(), r.begin());
        the_rest = std::span(r.begin(), r.end());
      } else {
        break;
      }
    }
    return result;
  }

private:
  auto _neighbours_of(Vertex v) const {
    return _span | std::views::filter([v](const auto& pair) {
             return pair.first == v || pair.second == v;
           });
  }

  void _dfs(std::unordered_map<Vertex, int>& visited, Vertex v,
            int mark_as) const {
    visited[v] = mark_as;
    for (const auto& [from, to] : _neighbours_of(v)) {
      if (visited[from] != mark_as) {
        _dfs(visited, from, mark_as);
      }
      if (visited[to] != mark_as) {
        _dfs(visited, to, mark_as);
      }
    }
  }

  std::optional<VertexCover> _vc_branch(int k, counter_t& count) const {
    ++count;
    // clang-format off
    if (k == 0) { return std::nullopt; }
    if (const auto first_edge = _find_pivot_edge()) {
      const auto [from, to] = *first_edge;
      auto cons = [](VertexCover vec, int val) { vec.push_back(val); return std::move(vec); };
      if (auto res = Graph(*this).remove_vertex(from)._vc_branch(k - 1, count)) { return cons(std::move(*res), from); }
      if (auto res = Graph(*this).remove_vertex(  to)._vc_branch(k - 1, count)) { return cons(std::move(*res),   to); }
      return std::nullopt;
    }
    return VertexCover();
    // clang-format on
  }

  std::optional<Edge>
  get_edge_bordering_vertex_of_degree_at_least_three() const {
    thread_local std::unordered_map<Vertex, std::size_t> degree;
    degree.clear();
    for (const auto& [from, to] : _span) {
      if (++degree[from] == 3 || ++degree[from] == 3) {
        return Edge{from, to};
      }
    }
    return std::nullopt;
  }

  std::optional<Edge> _first_edge() const {
    for (const auto& edge : _span) {
      return edge;
    }
    return std::nullopt;
  }

  std::optional<Edge> _find_pivot_edge() const {
    return get_edge_bordering_vertex_of_degree_at_least_three().or_else(
        [&]() { return _first_edge(); });
  }

  Graph& remove_vertex(Vertex v) {
    auto r = std::ranges::partition(_span, [v](const auto& pair) {
      return pair.first == v || pair.second == v;
    });
    _span = std::span(r.begin(), r.end());
    return *this;
  }

  using EdgesVector = std::vector<Edge>;
  std::shared_ptr<EdgesVector> _adj;
  std::span<Edge> _span;
};
