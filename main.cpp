#include <iostream>
#include <ranges>

enum Type : std::uint8_t {
    Foo,
    Bar,
    Baz
};


#pragma pack(push, 1)
struct MsgFoo {
    std::uint8_t size;
//    std::array<std::pair<std::uint8_t, std::uint8_t>, 0> arr;
    std::pair<std::uint8_t, std::uint8_t> arr[0];
};
static_assert(sizeof(MsgFoo) == sizeof(std::uint8_t));

struct MsgBar {

};

struct MsgBaz {

};
#pragma pack(pop)

int main() {

    return 0;
}
