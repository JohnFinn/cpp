#pragma once

#include <cstdint>
#include <utility>

enum Type : std::uint8_t { Foo, Bar, Baz };

#pragma pack(push, 1)
struct MsgFoo {
  std::uint8_t size;
  struct elem_t {
    std::uint8_t key;
    std::uint16_t value;
  };
  elem_t arr[0];
};

static_assert(sizeof(MsgFoo::elem_t) ==
              sizeof(std::uint8_t) + sizeof(std::uint16_t));

struct MsgBar {
  std::uint8_t id;
  std::uint8_t name[4];
};

struct MsgBaz {
  std::uint8_t a;
  std::uint8_t b;
  std::uint8_t c;
};
#pragma pack(pop)

constexpr std::size_t msg_size(const MsgFoo &msg) {
  return sizeof(MsgFoo) + msg.size;
};
constexpr std::size_t msg_size(const MsgBar &msg) { return sizeof(MsgBar); };
constexpr std::size_t msg_size(const MsgBaz &msg) { return sizeof(MsgBaz); };
