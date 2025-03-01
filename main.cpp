#include <charconv>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

std::string to_string(const auto &streamable) {
  std::ostringstream oss;
  oss << streamable;
  return oss.str();
}

int parse_int(std::string_view str) {
  int result;
  const auto [ptr, ec] = std::from_chars(str.begin(), str.end(), result);

  if (ec == std::errc()) {
    return result;
  }
  throw std::invalid_argument(std::string("Invalid number: ") +
                              to_string(std::quoted(str)));
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

auto split_first(auto &&range) {
  auto it = range.begin();
  return std::pair{*it, std::ranges::subrange(++it, range.end())};
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
    auto [first, rest] = split_first(input);
    const auto [num_verticies, num_edges] = parse_description(first);
    for (auto line : rest) {
      const auto [from, to] = parse_ints(line);
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
    return g;
  }

  std::map<int, std::vector<int>> _adj;
};

int main() {
  for (auto v : Graph().vertex_cover()) {
    std::cout << v << '\n';
  }
  return 0;
}
