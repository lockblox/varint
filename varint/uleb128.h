#pragma once
#include <cstddef>
#include <string_view>

namespace varint {

/** Unsigned Little-Endian Base128 codec
 *
 *  The most significant bit of each byte is used as a flag to indicate whether
 *  there are further bytes in the encoding.
 *  The remaining 7 bits contain the encoded value. */
class uleb128 {
 public:
  /** Encode an integral value into a varint */
  template <typename Integral>
  std::string encode(Integral input);

  /** Determine the number of bytes required to encode the input value */
  template <typename Integral>
  std::size_t encoded_size(Integral input);

  /** Encode an integral value into an output iterator */
  template <typename Integral, typename OutputIterator>
  std::size_t encode(Integral input, OutputIterator first);

  /** Decode an integral value from an iterator */
  template <typename InputIterator, typename Integral = std::size_t>
  Integral decode(InputIterator first, InputIterator last);

  /** Decode an integral value from a range */
  template <typename Range, typename Integral = std::size_t>
  Integral decode(Range input);
};  // namespace varint

/******************************************************************************/
/** IMPLEMENTATION */
/******************************************************************************/

template <typename Integral>
std::string uleb128::encode(Integral input) {
  static_assert(std::is_integral<Integral>::value);
  static_assert(!std::is_signed<Integral>::value);
  auto result = std::string{};
  encode(input, std::back_inserter(result));
  return result;
}

template <typename Integral, typename OutputIterator>
std::size_t uleb128::encode(Integral input, OutputIterator first) {
  static_assert(std::is_integral<Integral>::value);
  static_assert(!std::is_signed<Integral>::value);
  auto count = 1;
  while (input > 127) {
    *first++ = char(0x80 | input);
    input /= 128;
  }
  *first++ = char(input);
  return count;
}

template <typename InputIterator, typename Integral>
Integral uleb128::decode(InputIterator first, InputIterator last) {
  using value_type = typename std::iterator_traits<InputIterator>::value_type;
  static_assert(std::is_integral<value_type>::value && sizeof(value_type) == 1);
  static_assert(!std::is_signed<Integral>::value);
  Integral value = 0;
  Integral multiplier = 1;
  while ((*first & 0x80) != 0) {
    value += (*first++ & 0x7f) * multiplier;
    multiplier *= 128;
  }
  value += *first++ * multiplier;
  return value;
}

template <typename Range, typename Integral>
Integral uleb128::decode(Range input) {
  static_assert(std::is_integral<Integral>::value);
  static_assert(!std::is_signed<Integral>::value);
  using Iterator = typename Range::const_iterator;
  return decode<Iterator, Integral>(input.begin(), input.end());
}

/** TODO make this constexpr! */
template <typename Integral>
std::size_t uleb128::encoded_size(Integral input) {
  static_assert(std::is_integral<Integral>::value);
  static_assert(!std::is_signed<Integral>::value);
  std::size_t n = 1;
  while (input > 127) {
    ++n;
    input /= 128;
  }
  return n;
}

}  // namespace varint