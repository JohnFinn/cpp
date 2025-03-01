#include <map>
#include <optional>
#include <ranges>
#include <vector>

class Graph {
public:
  using Vertex = int;
  using Edge = std::pair<Vertex, Vertex>;

  template <std::ranges::range R>
    requires std::is_same_v<std::ranges::range_value_t<R>, Edge>
  Graph(R&& edges) {
    for (const auto e : edges) {
      _adj.push_back(e);
    }
  }

  using VertexCover = std::vector<Vertex>;

  VertexCover vertex_cover() const {
    for (std::size_t try_vertex_cover_size : std::views::iota(0)) {
      if (auto vc = _vc_branch(try_vertex_cover_size)) {
        return *vc;
      }
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

  std::optional<Edge> _first_edge() const {
    for (const auto& edge : _adj) {
      return edge;
    }
    return std::nullopt;
  }

  Graph remove_vertex(Vertex v) const {
    Graph g = *this;
    std::erase_if(g._adj, [v](const auto& pair) {
      return pair.first == v || pair.second == v;
    });
    return g;
  }

  std::vector<Edge> _adj;
};
