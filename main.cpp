#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

int main() {
  std::vector<std::pair<int, int>> pairs;
  std::string input(std::istreambuf_iterator<char>{std::cin},
                    {}); // Read entire stdin as a string

  for (auto line :
       input | std::views::split('\n') |
           std::views::filter([](auto line) { return *line.begin() != 'c'; })) {
    std::cout << std::string{line.begin(), line.end()}
              << '\n'; // Convert view to string
  }
  return 0;
}
