#pragma once
#include <string>

namespace varint {

/** Compare a std::array version with a std::string_view version?
 * 
 * Compare equality with underlying container 
 * 
 * Only want to compare equality with same type, but compare with integrals
 * unsigned and signed
 * 
 * Reading a varint from a stream? Pair of iterators */

/** Provides a zero-copy view onto a buffer containing a varint-encoded value */
template <typename Codec, typename Container = std::string>
class varint {
 public:
  static_assert(std::is_same<Container::value_type, char>::value);

  /** Default constructor */
  varint();

  /** Create from a buffer */
  explicit varint(Container data);

  /** Create from an integral value */
  template <typename Integral>
  explicit varint(Integral value);

  /** Assign an integral value */
  template <typename Integral>
  varint& operator=(Integral value);

  /** Convert to integral value */
  template <typename Integral>
  explicit operator Integral() const;

  /** Compare with another varint */
  bool operator==(const varint& rhs);

  /** Compare with an integral value */
  template <typename Integral>
  bool operator==(Integral) const;

  /** Get the encoded data */
  const char* data() const;

  /** Get the number of bytes in the encoded data */
  std::size_t size() const;

 private:
  Container data_;
};

template <typename Codec, typename Container>
varint<Codec, Container>::varint() {}

template <typename Codec, typename Container>
varint<Codec, Container>::varint(Container data) : data_(std::move(data)) {}

template <typename Codec, typename Container>
template <typename Integral>
varint<Codec, Container>::varint(Integral value) {
  *this = value;
}

/** TODO use SFINAE to specialise for string/vector (has allocator_type) to
 * enable back inserter */
template <typename Codec, typename Container>
template <typename Integral>
varint<Codec, Container>& varint<Codec, Container>::operator=(Integral value) {
  if (size() < Codec::encoded_size(value)) {
    throw std::overflow_error("value " + std::to_string(value) +
                              " too large for varint buffer of size " +
                              std::to_string(size()));
  }
  Codec::encode(value, std::begin(data_));
}

template <typename Codec, typename Container>
template <typename Integral>
varint<Codec, Container>::operator Integral() const {
  static_assert(std::is_integral<Integral>::value);
  return Codec::decode<Integral>(std::begin(data_), std::end(data_));
}

template <typename Codec, typename Container>
bool varint<Codec, Container>::operator==(const varint& rhs) {
  return size() == rhs.size() &&
         std::equal(std::begin(data_), std::end(data_), std::begin(rhs.data_));
}

template <typename Codec, typename Container>
const char* varint<Codec, Container>::data() const {
  return data_.data();
}

template <typename Codec, typename Container>
std::size_t varint<Codec, Container>::size() const {
  return data_.size();
}

}  // namespace varint