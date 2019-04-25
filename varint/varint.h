#pragma once
#include <varint/codecs/uleb128.h>
#include <varint/detail/extent_type.h>
#include <iterator>
#include <string>

namespace varint {

/** Value type holding a varint-encoded integral value */
template <typename Codec, typename Container>
class varint {
 public:
  static_assert(std::is_class<Container>::value);
  static_assert(std::is_same<typename Container::value_type, char>::value);

  /** Default constructor */
  varint();

  /** Returns an iterator to the beginning of the encoding */
  auto begin() { return std::begin(data_); }
  auto begin() const { return std::begin(data_); }
  auto cbegin() const { return std::begin(data_); }

  /** Returns an iterator to the end of the encoding */
  auto end() { return std::end(data_); }
  auto end() const { return std::end(data_); }
  auto cend() const { return std::end(data_); }

  /** Read from a stream */
  template <typename CodecT, typename ContainerT>
  friend std::istream& operator>>(std::istream& is,
                                  varint<CodecT, ContainerT>& vi);

  /** Write to a stream */
  template <typename CodecT, typename ContainerT>
  friend std::ostream& operator<<(std::ostream& os,
                                  varint<CodecT, ContainerT>& vi);

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

  /** Get the encoded data */
  constexpr const char* data() const;

  /** Get the number of bytes in the encoded data */
  constexpr std::size_t size() const;

 private:
  auto output_iterator(detail::dynamic_extent_t);
  auto output_iterator(detail::static_extent_t);

  template <typename Integral>
  void assign(Integral value, detail::static_extent_t extent);

  template <typename Integral>
  void assign(Integral value, detail::dynamic_extent_t extent);

  Container data_;
};

/** Compare varints with the same codec */

template <typename Codec, typename ContainerL, typename ContainerR>
bool operator==(const varint<Codec, ContainerL>& lhs,
                const varint<Codec, ContainerR>& rhs);

template <typename Codec, typename ContainerL, typename ContainerR>
bool operator!=(const varint<Codec, ContainerL>& lhs,
                const varint<Codec, ContainerR>& rhs);

template <typename Codec, typename ContainerL, typename ContainerR>
bool operator<(const varint<Codec, ContainerL>& lhs,
               const varint<Codec, ContainerR>& rhs);

template <typename Codec, typename ContainerL, typename ContainerR>
bool operator>(const varint<Codec, ContainerL>& lhs,
               const varint<Codec, ContainerR>& rhs);

template <typename Codec, typename ContainerL, typename ContainerR>
bool operator<=(const varint<Codec, ContainerL>& lhs,
                const varint<Codec, ContainerR>& rhs);

template <typename Codec, typename ContainerL, typename ContainerR>
bool operator>=(const varint<Codec, ContainerL>& lhs,
                const varint<Codec, ContainerR>& rhs);

template <typename Container = std::string>
using uleb128 = varint<codecs::uleb128, Container>;

/******************************************************************************/
/** IMPLEMENTATION */
/******************************************************************************/

template <typename Codec, typename Container>
varint<Codec, Container>::varint() {}

template <typename Codec, typename Container>
varint<Codec, Container>::varint(Container data) : data_(std::move(data)) {}

template <typename Codec, typename Container>
template <typename Integral>
varint<Codec, Container>::varint(Integral value) {
  *this = value;
}

template <typename Codec, typename Container>
template <typename Integral>
varint<Codec, Container>& varint<Codec, Container>::operator=(Integral value) {
  using extent_type = typename detail::extent_type<Container>::type;
  assign(value, extent_type{});
  return *this;
}

template <typename Codec, typename Container>
auto varint<Codec, Container>::output_iterator(detail::dynamic_extent_t) {
  return std::back_inserter(data_);
}

template <typename Codec, typename Container>
auto varint<Codec, Container>::output_iterator(detail::static_extent_t) {
  return std::begin(data_);
}

template <typename Codec, typename Container>
template <typename Integral>
void varint<Codec, Container>::assign(Integral value,
                                      detail::static_extent_t extent) {
  static_assert(std::is_signed<Integral>::value ==
                std::is_signed<Codec>::value);
  if (size() < Codec::size(value)) {
    throw std::out_of_range("value " + std::to_string(value) +
                            " too large for varint buffer of size " +
                            std::to_string(size()));
  }
  Codec::encode(value, output_iterator(extent));
}

template <typename Codec, typename Container>
template <typename Integral>
void varint<Codec, Container>::assign(Integral value,
                                      detail::dynamic_extent_t extent) {
  static_assert(std::is_signed<Integral>::value ==
                std::is_signed<Codec>::value);
  data_.clear();
  data_.resize(Codec::encode(value, output_iterator(extent)));
}

template <typename Codec, typename Container>
template <typename Integral>
varint<Codec, Container>::operator Integral() const {
  static_assert(std::is_integral<Integral>::value,
                "cannot cast to non-integral type");
  using InputIterator = decltype(std::begin(data_));
  return Codec::template decode<InputIterator, Integral>(std::begin(data_),
                                                         std::end(data_));
}

template <typename Codec, typename ContainerL, typename ContainerR>
bool operator==(const varint<Codec, ContainerL>& lhs,
                const varint<Codec, ContainerR>& rhs) {
  return Codec::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Codec, typename ContainerL, typename ContainerR>
bool operator!=(const varint<Codec, ContainerL>& lhs,
                const varint<Codec, ContainerR>& rhs) {
  return !(lhs == rhs);
}

template <typename Codec, typename ContainerL, typename ContainerR>
bool operator<(const varint<Codec, ContainerL>& lhs,
               const varint<Codec, ContainerR>& rhs) {
  return Codec::less(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Codec, typename ContainerL, typename ContainerR>
bool operator>(const varint<Codec, ContainerL>& lhs,
               const varint<Codec, ContainerR>& rhs) {
  return rhs < lhs;
}

template <typename Codec, typename ContainerL, typename ContainerR>
bool operator<=(const varint<Codec, ContainerL>& lhs,
                const varint<Codec, ContainerR>& rhs) {
  return !(lhs > rhs);
}

template <typename Codec, typename ContainerL, typename ContainerR>
bool operator>=(const varint<Codec, ContainerL>& lhs,
                const varint<Codec, ContainerR>& rhs) {
  return !(lhs < rhs);
}

template <typename Codec, typename Container>
constexpr const char* varint<Codec, Container>::data() const {
  return data_.data();
}

template <typename Codec, typename Container>
constexpr std::size_t varint<Codec, Container>::size() const {
  return data_.size();
}

template <typename Codec, typename Container>
std::istream& operator>>(std::istream& is, varint<Codec, Container>& vi) {
  using extent_type = typename detail::extent_type<Container>::type;
  Codec::copy(std::istreambuf_iterator<char>(is),
              std::istreambuf_iterator<char>(),
              vi.output_iterator(extent_type()));
  return is;
}

template <typename Codec, typename Container>
std::ostream& operator<<(std::ostream& os, varint<Codec, Container>& vi) {
  std::copy(std::begin(vi.data_), std::end(vi.data_),
            std::ostreambuf_iterator<char>(os));
  return os;
}

}  // namespace varint