#include "graph.hpp"

#include "structopt/app.hpp"
#include <charconv>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <ranges>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/wait.h>
#include <thread>
#include <type_traits>
#include <unistd.h>

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

class SubprocessHandle {
  pid_t _pid;
  explicit SubprocessHandle(pid_t pid) : _pid(pid) {}

public:
  static std::optional<SubprocessHandle> fork() {
    if (int pid = ::fork(); pid > 0) {
      return SubprocessHandle(pid);
    } else if (pid == 0) {
      return std::nullopt;
    } else {
      throw std::runtime_error("fork failed");
    }
  };

  ~SubprocessHandle() { ::waitpid(_pid, nullptr, 0); }
};

struct pipe_fds {
private:
  int _fds[2];

  int read_fd() const { return _fds[0]; }
  int write_fd() const { return _fds[1]; }

public:
  pipe_fds() {
    if (pipe(_fds) != 0) {
      throw std::runtime_error("pipe failed");
    }
  }

  void read(std::span<std::byte> span) {
    if (::read(read_fd(), span.data(), span.size_bytes()) !=
        span.size_bytes()) {
      throw std::runtime_error("read failed");
    }
  }
  void write(std::span<const std::byte> span) {
    if (::write(write_fd(), span.data(), span.size_bytes()) !=
        span.size_bytes()) {
      throw std::runtime_error("write failed");
    }
  }
};

template <typename F>
  requires std::is_trivially_copyable_v<std::invoke_result_t<F>>
const auto timeout(std::chrono::seconds duration, F f) {
  pipe_fds p;
  using res_t = std::invoke_result_t<F>;
  if (auto subprocess = SubprocessHandle::fork(); subprocess.has_value()) {
    std::array<std::byte, sizeof(res_t)> buffer;
    auto before = std::chrono::high_resolution_clock::now();
    // TODO:: implement timeouting
    p.read(buffer);
    return std::bit_cast<res_t>(buffer);
  } else {
    const auto result = f();
    const auto arr =
        std::bit_cast<std::array<std::byte, sizeof(res_t)>>(result);
    p.write(std::span(arr));
    std::exit(0);
  }
};

const auto measure_time(auto f) {
  auto before = std::chrono::high_resolution_clock::now();
  auto result = f();
  auto after = std::chrono::high_resolution_clock::now();
  return std::pair(after - before, result);
};

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

          const auto time = timeout(std::chrono::seconds(10), [&graph] {
            return measure_time(
                       [&graph] { return graph.vertex_cover().size(); })
                .first;
          });
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
