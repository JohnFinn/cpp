#include "graph.hpp"
#include "util.hpp"
#include <fstream>

#include "structopt/app.hpp"

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

          const auto result = timeout(std::chrono::seconds(10), [&graph] {
            return measure_time(
                [&graph] { return graph.vertex_cover().size(); });
          });

          std::cout << graph.edges().size() << ','
                    << result.transform([](auto r) { return r.time; })
                           .value_or(std::chrono::nanoseconds(-1))
                           .count()
                    << std::endl;
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
