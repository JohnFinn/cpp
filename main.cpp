#include "graph.hpp"

#include "structopt/app.hpp"
#include <charconv>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>

std::string to_string(const auto& streamable) {
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

auto split_first(auto&& range) {
  auto it = range.begin();
  return std::pair{*it, std::ranges::subrange(++it, range.end())};
}

Graph parse_graph(std::string_view str) {
  auto input = str | std::views::split('\n') |
               std::views::transform([](auto word) {
                 return std::string_view(word.begin(), word.end());
               }) |
               std::views::filter([](auto line) {
                 return line.size() > 0 && !line.starts_with("c");
               });
  const auto [first, rest] = split_first(input);
  const auto [num_verticies, num_edges] = parse_description(first);
  return Graph(rest | std::views::transform(parse_ints));
}

struct Options {
  std::optional<std::vector<std::string>> timegraph;
};
STRUCTOPT(Options, timegraph);

int main(int argc, char **argv) {
  const auto args = [argc, argv]() {
    try {
      return structopt::app("graph").parse<Options>(argc, argv);
    } catch (const structopt::exception& e) {
      std::cerr << e.what() << '\n';
      std::cerr << e.help() << '\n';
      std::exit(1);
    }
  }();

  return args.timegraph
      .transform([](const auto& timegraph) {
        std::ranges::for_each(timegraph, [](const auto& file) {
          std::ifstream fin(file);
          const auto graph =
              parse_graph(std::string(std::istreambuf_iterator<char>{fin}, {}));
          const auto measure_time = [](auto f) {
            auto before = std::chrono::high_resolution_clock::now();
            auto result = f();
            auto after = std::chrono::high_resolution_clock::now();
            return std::pair(after - before, result);
          };
          const auto [time, vertex_cover] =
              measure_time([graph] { return graph.vertex_cover(); });
          std::cout << graph.edges().size() << ',' << time.count() << '\n';
        });
        return 0;
      })
      .or_else([] {
        const auto graph = parse_graph(
            std::string(std::istreambuf_iterator<char>{std::cin}, {}));
        for (auto v : graph.vertex_cover()) {
          std::cout << v << '\n';
        }
        return std::optional<int>(0);
      })
      .value();
}
