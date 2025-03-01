#include "graph.hpp"

#include <charconv>
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

int main() {
  auto input = std::string(std::istreambuf_iterator<char>{std::cin}, {}) |
               std::views::split('\n') | std::views::transform([](auto word) {
                 return std::string_view(word.begin(), word.end());
               }) |
               std::views::filter([](auto line) {
                 return line.size() > 0 && !line.starts_with("c");
               });
  auto [first, rest] = split_first(input);
  const auto [num_verticies, num_edges] = parse_description(first);
  for (auto v :
       Graph(rest | std::views::transform(parse_ints)).vertex_cover()) {
    std::cout << v << '\n';
  }
  return 0;
}
