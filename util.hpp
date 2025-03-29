#include <charconv>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <optional>
#include <poll.h>
#include <signal.h>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <sys/wait.h>
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

class SubprocessHandle {
  pid_t _pid;

public:
  explicit SubprocessHandle(pid_t pid) : _pid(pid) {}

  static std::optional<SubprocessHandle> fork() {
    if (int pid = ::fork(); pid > 0) {
      return std::optional<SubprocessHandle>(pid);
    } else if (pid == 0) {
      return std::nullopt;
    } else {
      throw std::runtime_error("fork failed");
    }
  };

  SubprocessHandle(const SubprocessHandle&) = delete;
  SubprocessHandle& operator=(const SubprocessHandle&) = delete;
  SubprocessHandle(SubprocessHandle&&) = delete;
  SubprocessHandle& operator=(SubprocessHandle&&) = delete;

  void kill() { ::kill(_pid, SIGKILL); }

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

  bool poll_read(std::chrono::milliseconds timeout) {
    pollfd pfd{.fd = read_fd(), .events = POLLIN, .revents = 0};
    if (auto ret = ::poll(&pfd, 1, timeout.count()); ret > 0) {
      return true;
    } else if (ret == 0) {
      return false;
    } else {
      throw std::runtime_error("poll failed");
    }
  }
};

template <typename F>
  requires std::is_trivially_copyable_v<std::invoke_result_t<F>>
const std::optional<std::invoke_result_t<F>>
timeout(std::chrono::seconds duration, F f) {
  pipe_fds p;
  using res_t = std::invoke_result_t<F>;
  using buffer_t = std::array<std::byte, sizeof(res_t)>;
  if (auto subprocess = SubprocessHandle::fork(); subprocess.has_value()) {
    buffer_t buffer;
    if (p.poll_read(duration)) {
      p.read(buffer);
      return std::bit_cast<res_t>(buffer);
    }
    std::cout << "killing\n";
    subprocess->kill();
    return std::nullopt;
  } else {
    const auto result = f();
    const auto arr = std::bit_cast<buffer_t>(result);
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
