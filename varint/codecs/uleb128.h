#pragma once
#include <cstddef>
#include <stdexcept>
#include <string_view>

namespace varint {
namespace codecs {

/** Unsigned Little-Endian Base128 codec
 *
 *  The most significant bit of each byte is used as a flag to indicate whether
 *  there are further bytes in the encoding.
 *  The remaining 7 bits contain the encoded value. */
class uleb128 {
 public:
  /** Determine the number of bytes required to encode the input value */
  template <typename Integral>
  static constexpr std::size_t size(Integral input);

  /** Determine the size in bytes of encoded data */
  template <typename InputIterator>
  static constexpr std::size_t size(InputIterator first, InputIterator last);

  /** Encode an integral value into an output iterator */
  template <typename Integral, typename OutputIterator>
  static std::size_t encode(Integral input, OutputIterator first);

  /** Decode an integral value from an iterator */
  template <typename InputIterator, typename Integral = std::size_t>
  static Integral decode(InputIterator first, InputIterator last);

  /** Copy encoded value from input to output */
  template <typename InputIterator, typename OutputIterator>
  static OutputIterator copy(InputIterator first, InputIterator last,
                             OutputIterator output);

  /** Compare two encoded ranges */
  static bool less(std::string_view lhs, std::string_view rhs);

  /** Compare two encoded ranges for inequality */
  static bool equal(std::string_view lhs, std::string_view rhs);

 private:
  template <typename Integral>
  static Integral add(Integral lhs, std::size_t delta);
};

/******************************************************************************/
/** IMPLEMENTATION */
/******************************************************************************/

template <typename Integral>
Integral uleb128::add(Integral value, std::size_t delta) {
  static constexpr auto max = std::numeric_limits<Integral>::max();
  auto result = value + delta;
  if (result > max) {
    throw std::overflow_error("value exceeds capacity of destination");
  }
  return static_cast<Integral>(result);
}

template <typename Integral, typename OutputIterator>
std::size_t uleb128::encode(Integral input, OutputIterator first) {
  static_assert(std::is_integral<Integral>::value,
                "cannot encode non-integral type");
  static_assert(!std::is_signed<Integral>::value,
                "cannot encode signed value using unsigned codec");
  auto count = 1;
  while (input > 127) {
    *first++ = char(0x80 | input);
    input /= 128;
    ++count;
  }
  *first++ = char(input);
  return count;
}

template <typename InputIterator, typename Integral>
Integral uleb128::decode(InputIterator first, InputIterator last) {
  using value_type = typename std::iterator_traits<InputIterator>::value_type;
  static_assert(std::is_integral<value_type>::value,
                "value_type of iterator must be an integral type");
  static_assert(sizeof(value_type) == 1,
                "value_type of iterator must be byte sized");
  Integral value = 0;
  Integral multiplier = 1;
  while (first != last && (*first & 0x80) != 0) {
    value = add(value, (*first++ & 0x7f) * multiplier);
    multiplier *= 128;
  }
  if (first == last) {
    throw std::out_of_range("incomplete varint encoding");
  }
  return add(value, *first++ * multiplier);
}

template <typename Integral>
constexpr std::size_t uleb128::size(Integral input) {
  static_assert(std::is_integral<Integral>::value,
                "cannot compute encoded size of non-integral type");
  static_assert(!std::is_signed<Integral>::value,
                "cannot compute size of signed value with unsigned codec");
  std::size_t n = 1;
  for (; input > 127; input /= 128, ++n)
    ;
  return n;
}

template <typename InputIterator>
constexpr std::size_t uleb128::size(InputIterator first, InputIterator last) {
  using value_type = typename std::iterator_traits<InputIterator>::value_type;
  static_assert(std::is_integral<value_type>::value,
                "value_type of iterator must be an integral type");
  static_assert(sizeof(value_type) == 1,
                "value_type of iterator must be byte sized");
  if (first == last) return 0;
  for (std::size_t result = 1; first != last; ++first, ++result) {
    if ((*first & 0x80) == 0) {
      return result;
    }
  }
  return 0;
}

template <typename InputIterator, typename OutputIterator>
OutputIterator uleb128::copy(InputIterator first, InputIterator last,
                             OutputIterator output) {
  using value_type = typename std::iterator_traits<InputIterator>::value_type;
  static_assert(std::is_integral<value_type>::value,
                "value_type of iterator must be an integral type");
  static_assert(sizeof(value_type) == 1,
                "value_type of iterator must be byte sized");
  if (first == last) return output;
  do {
    *output++ = *first;
  } while (*first++ & 0x80 && first != last);
  return output;
}

}  // namespace codecs
}  // namespace varint

namespace std {

template <>
class is_signed<varint::codecs::uleb128> {
 public:
  const static bool value = false;
};

}  // namespace std