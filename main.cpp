#include "msg_stream_iterator.h"
#include <ranges>


class MsgView : public std::ranges::view_interface<MsgView> {
public:
    MsgView(std::span<std::uint8_t> span) : _span(span) {}

    auto begin() {
        return msg_stream_iter(_span.data(), _span.size());
    }

    auto end() {
        return msg_stream_iter(_span.data() + _span.size(), 0);
    }

private:
    std::span<std::uint8_t> _span;
};

int main() {

    return 0;
}
