#include <charconv>
#include <iomanip>
#include <iostream>
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

int main() {
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
      const auto [n, m] = parse_description(line);
      std::cout << n << " " << m << std::endl;
      is_first = false;
      continue;
    }
    const auto [a, b] = parse_ints(line);
    std::cout << a << " " << b << std::endl;
  }
  return 0;
}
