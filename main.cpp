#include <charconv>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

std::string quoted_string(std::string_view str) {
  std::ostringstream oss;
  oss << std::quoted(str);
  return oss.str();
}

int parse_int(std::string_view str) {
  int result;
  auto [ptr, ec] = std::from_chars(str.begin(), str.end(), result);

  if (ec == std::errc())
    return result;
  throw std::invalid_argument(std::string("Invalid number: ") +
                              quoted_string(str));
}

std::pair<int, int> parse_ints(std::string_view line) {
  const auto space = line.find(' ');
  return {parse_int(line.substr(0, space)), parse_int(line.substr(space + 1))};
}

std::pair<int, int> parse_description(std::string_view line) {
  const auto skip = sizeof("p td");
  const auto space = line.find(' ', skip);
  return {parse_int(line.substr(skip, space)),
          parse_int(line.substr(space + 1))};
}

class Graph {
public:
  Graph() {
    auto input = std::string(std::istreambuf_iterator<char>{std::cin}, {}) |
                 std::views::split('\n') | std::views::transform([](auto word) {
                   return std::string_view(word.begin(), word.end());
                 }) |
                 std::views::filter([](auto line) {
                   return line.size() > 0 && !line.starts_with("c");
                 });

    bool is_first = true;
    for (auto line : input) {
      if (is_first) {
        const auto [num_verticies, num_edges] = parse_description(line);
        is_first = false;
        continue;
      }
      const auto [from, to] = parse_ints(line);
      _adj[from].push_back(to);
      _adj[to].push_back(from);
    }
    printGraph(_adj);
  }

  using VertexCover = std::vector<int>;

  VertexCover vertex_cover() const {
    // return *_vc_branch(2);
    std::size_t try_vertex_cover_size = 0;
    while (true) {
      if (auto vc = _vc_branch(try_vertex_cover_size)) {
        return *vc;
      }
      try_vertex_cover_size += 1;
    }
  }

  std::size_t num_edges() const {
    auto sizes = _adj | std::views::transform(
                            [](const auto &v) { return v.second.size(); });
    return std::accumulate(sizes.begin(), sizes.end(), 0);
  }

private:
  std::optional<VertexCover> _vc_branch(int k) const {
    if (k == 0) {
      return std::nullopt;
    }
    if (num_edges() == 0) {
      return VertexCover();
    }
    const auto [from, to] = _first_edge().value();
    if (auto res = remove_vertex(from)._vc_branch(k - 1)) {
      res.value().push_back(from);
      return res;
    }
    if (auto res = remove_vertex(to)._vc_branch(k - 1)) {
      res.value().push_back(to);
      return res;
    }
    return std::nullopt;
  }

  using Edge = std::pair<std::size_t, std::size_t>;
  std::optional<Edge> _first_edge() const {
    for (const auto &[v, edges] : _adj) {
      if (!edges.empty()) {
        return std::pair(v, edges.front());
      }
    }
    return std::nullopt;
  }

  Graph remove_vertex(std::size_t v) const {
    Graph g = *this;
    g._adj.erase(v);
    for (auto &[_, edges] : g._adj) {
      auto view = edges | std::views::filter([v](auto e) { return e != v; });
      edges = std::vector(view.begin(), view.end());
    }
    std::cout << "-------------\n";
    std::cout << "Removed vertex " << v << std::endl;
    printGraph(g._adj);
    std::cout << "-------------\n";
    return g;
  }

  static void printGraph(const std::map<int, std::vector<int>> &graph) {
    for (const auto &[vertex, neighbors] : graph) {
      // Print the vertex label (this represents the node in the graph)
      std::cout << "Vertex " << vertex << " --> ";

      // Print the neighbors for that vertex (edges)
      if (neighbors.empty()) {
        std::cout << "No neighbors";
      } else {
        for (size_t i = 0; i < neighbors.size(); ++i) {
          std::cout << neighbors[i];
          if (i < neighbors.size() - 1) {
            std::cout << ", ";
          }
        }
      }

      // Newline after each vertex's neighbors
      std::cout << std::endl;
    }
  }

  std::map<int, std::vector<int>> _adj;
};

int main() {
  for (auto v : Graph().vertex_cover()) {
    std::cout << v << '\n';
  }
  return 0;
}
