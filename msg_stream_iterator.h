#pragma once

#include "messages.h"
#include <iterator>
#include <stdexcept>
#include <variant>

class msg_stream_iter {
public:
  using difference_type = std::ptrdiff_t;
  using value_type = std::variant<std::reference_wrapper<const MsgFoo>,
                                  std::reference_wrapper<const MsgBar>,
                                  std::reference_wrapper<const MsgBaz>>;

  msg_stream_iter(const void *ptr, std::size_t size)
      : _current(static_cast<const std::uint8_t *>(ptr)), _size(size) {}

  msg_stream_iter() : msg_stream_iter(nullptr, 0) {}

  value_type operator*() { return _get(); }

  bool operator==(const msg_stream_iter &other) const {
    return _current == other._current;
  }

  msg_stream_iter operator++(int) const {
    return std::visit(
        [this](const auto &msg) {
          const auto header_and_msg_size = sizeof(Type) + msg_size(msg.get());
          return msg_stream_iter(_current + header_and_msg_size,
                                 _size - header_and_msg_size);
        },
        _get());
  }

  msg_stream_iter &operator++() { return *this = (*this)++; }

private:
  value_type _get() const {
    switch (*reinterpret_cast<const Type *>(_current)) {
    case Foo:
      return *reinterpret_cast<const MsgFoo *>(_current + 1);
    case Bar:
      return *reinterpret_cast<const MsgBar *>(_current + 1);
    case Baz:
      return *reinterpret_cast<const MsgBaz *>(_current + 1);
    default:
      throw std::runtime_error("unexpected message");
    }
  }

  const std::uint8_t *_current;
  std::size_t _size;
};
