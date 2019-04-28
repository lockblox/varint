#pragma once
#include <varint/basic_varint.h>
#include <array>
#include <string>
#include <string_view>

namespace varint {

template <typename Container = std::string>
using uleb128 = basic_varint<codecs::uleb128, Container>;

template <typename Codec>
using string = basic_varint<Codec, std::string>;

template <typename Codec>
using string_view = basic_varint<Codec, std::string_view>;

template <typename Codec, std::size_t N>
class array : public basic_varint<Codec, std::array<char, Codec::size(N)>> {
 public:
  array() : basic_varint<Codec, std::array<char, Codec::size(N)>>{N} {};
};

}  // namespace varint