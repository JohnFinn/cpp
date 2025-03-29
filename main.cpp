#include "BS_thread_pool.hpp"
#include "graph.hpp"
#include "util.hpp"
#include <fstream>
#include <syncstream>

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
        BS::thread_pool pool(8);
        auto to_vector = [](std::ranges::sized_range auto&& range) {
          return std::vector(range.begin(), range.end());
        };
        auto futures = to_vector(
            timegraph | std::views::transform([&pool](const auto& file) {
              std::ifstream fin(file);
              const auto graph = parse_graph(
                  std::string(std::istreambuf_iterator<char>{fin}, {}));

              return std::pair(graph.edges().size(), pool.submit_task([graph] {
                return timeout(std::chrono::seconds(60), [graph] {
                  return measure_time(
                      [graph] { return graph.vertex_cover().first; });
                });
              }));
            }));

        { std::osyncstream{std::cout} << "num_edges,time_ns,num_rec\n"; }
        for (auto& [size, future] : futures) {
          std::osyncstream{std::cout} << size << ',' <<
              [](const auto& pair) {
                auto [time, count] = pair;
                return std::format("{},{}", time.count(), count);
              }(future.get().value_or(measured_t<Graph::counter_t>(
                  std::chrono::nanoseconds(-1), -1)))
                                      << std::endl;
        }

        return 0;
      })
      .or_else([] {
        const auto graph = parse_graph(
            std::string(std::istreambuf_iterator<char>{std::cin}, {}));
        for (auto v : graph.vertex_cover().second) {
          std::cout << v << '\n';
        }
        return std::optional<int>(0);
      })
      .value();
}
