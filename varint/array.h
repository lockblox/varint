#pragma once
#include <varint/varint.h>
#include <cstddef>

namespace varint {

template <typename Codec, std::size_t N>
class array : public varint<Codec, std::array<char, Codec::size(N)>> {
 public:
  array() : varint<Codec, std::array<char, Codec::size(N)>>{N} {};
};

}  // namespace varint