#include <map>
#include <optional>
#include <ranges>
#include <vector>

class Graph {
public:
  Graph(auto&& edges) {
    for (const auto [from, to] : edges) {
      _adj[from].push_back(to);
      _adj[to].push_back(from);
    }
  }

  using VertexCover = std::vector<int>;

  VertexCover vertex_cover() const {
    std::size_t try_vertex_cover_size = 0;
    while (true) {
      if (auto vc = _vc_branch(try_vertex_cover_size)) {
        return *vc;
      }
      try_vertex_cover_size += 1;
    }
  }

private:
  std::optional<VertexCover> _vc_branch(int k) const {
    // clang-format off
    if (k == 0) { return std::nullopt; }
    if (const auto first_edge = _first_edge()) {
      const auto [from, to] = *first_edge;
      auto cons = [](VertexCover vec, int val) { vec.push_back(val); return std::move(vec); };
      if (auto res = remove_vertex(from)._vc_branch(k - 1)) { return cons(std::move(*res), from); }
      if (auto res = remove_vertex(  to)._vc_branch(k - 1)) { return cons(std::move(*res),   to); }
      return std::nullopt;
    }
    return VertexCover();
    // clang-format on
  }

  using Edge = std::pair<std::size_t, std::size_t>;
  std::optional<Edge> _first_edge() const {
    for (const auto& [v, edges] : _adj) {
      if (!edges.empty()) {
        return std::pair(v, edges.front());
      }
    }
    return std::nullopt;
  }

  Graph remove_vertex(std::size_t v) const {
    Graph g = *this;
    g._adj.erase(v);
    for (auto& [_, edges] : g._adj) {
      auto view = edges | std::views::filter([v](auto e) { return e != v; });
      edges = std::vector(view.begin(), view.end());
    }
    return g;
  }

  std::map<int, std::vector<int>> _adj;
};
