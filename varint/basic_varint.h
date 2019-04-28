#pragma once
#include <varint/codecs/uleb128.h>
#include <varint/detail/extent_type.h>
#include <iterator>
#include <string>

namespace varint {

/** Value type holding a varint-encoded integral value */
template <typename Codec, typename Container>
class basic_varint {
 public:
  static_assert(std::is_class<Container>::value);
  static_assert(std::is_same<typename Container::value_type, char>::value);

  /** Default constructor */
  basic_varint();

  /** Read from a stream */
  template <typename CodecT, typename ContainerT>
  friend std::istream& operator>>(std::istream& is,
                                  basic_varint<CodecT, ContainerT>& vi);

  /** Write to a stream */
  template <typename CodecT, typename ContainerT>
  friend std::ostream& operator<<(std::ostream& os,
                                  basic_varint<CodecT, ContainerT>& vi);

  /** Create from a buffer */
  explicit basic_varint(Container data);

  /** Create from an integral value */
  template <typename Integral>
  explicit basic_varint(Integral value);

  /** Assign an integral value */
  template <typename Integral>
  basic_varint& operator=(Integral value);

  /** Convert to integral value */
  template <typename Integral>
  explicit operator Integral() const;

  /** Convert to string_view */
  explicit operator std::string_view() const;

 private:
  template <typename T>
  T get(std::true_type is_integral) const;

  template <typename T>
  std::string_view get(std::false_type is_integral) const;

  auto output_iterator(detail::dynamic_extent_t);
  auto output_iterator(detail::static_extent_t);

  template <typename Integral>
  void assign(Integral value, detail::static_extent_t extent);

  template <typename Integral>
  void assign(Integral value, detail::dynamic_extent_t extent);

  Container data_;
  std::size_t size_;
};

/** Compare varints with the same codec */

template <typename Codec, typename ContainerL, typename ContainerR>
bool operator==(const basic_varint<Codec, ContainerL>& lhs,
                const basic_varint<Codec, ContainerR>& rhs);

template <typename Codec, typename ContainerL, typename ContainerR>
bool operator!=(const basic_varint<Codec, ContainerL>& lhs,
                const basic_varint<Codec, ContainerR>& rhs);

template <typename Codec, typename ContainerL, typename ContainerR>
bool operator<(const basic_varint<Codec, ContainerL>& lhs,
               const basic_varint<Codec, ContainerR>& rhs);

template <typename Codec, typename ContainerL, typename ContainerR>
bool operator>(const basic_varint<Codec, ContainerL>& lhs,
               const basic_varint<Codec, ContainerR>& rhs);

template <typename Codec, typename ContainerL, typename ContainerR>
bool operator<=(const basic_varint<Codec, ContainerL>& lhs,
                const basic_varint<Codec, ContainerR>& rhs);

template <typename Codec, typename ContainerL, typename ContainerR>
bool operator>=(const basic_varint<Codec, ContainerL>& lhs,
                const basic_varint<Codec, ContainerR>& rhs);

/******************************************************************************/
/** IMPLEMENTATION */
/******************************************************************************/

template <typename Codec, typename Container>
basic_varint<Codec, Container>::basic_varint() {}

template <typename Codec, typename Container>
basic_varint<Codec, Container>::basic_varint(Container data)
    : data_(std::move(data)), size_(Codec::size(data_.begin(), data_.end())) {}

template <typename Codec, typename Container>
template <typename Integral>
basic_varint<Codec, Container>::basic_varint(Integral value) {
  *this = value;
}

template <typename Codec, typename Container>
template <typename Integral>
basic_varint<Codec, Container>& basic_varint<Codec, Container>::operator=(
    Integral value) {
  using extent_type = typename detail::extent_type<Container>::type;
  assign(value, extent_type{});
  return *this;
}

template <typename Codec, typename Container>
auto basic_varint<Codec, Container>::output_iterator(detail::dynamic_extent_t) {
  return std::back_inserter(data_);
}

template <typename Codec, typename Container>
auto basic_varint<Codec, Container>::output_iterator(detail::static_extent_t) {
  return std::begin(data_);
}

template <typename Codec, typename Container>
template <typename Integral>
void basic_varint<Codec, Container>::assign(Integral value,
                                            detail::static_extent_t extent) {
  static_assert(std::is_signed<Integral>::value ==
                std::is_signed<Codec>::value);
  if (data_.size() < Codec::size(value)) {
    throw std::out_of_range("value " + std::to_string(value) +
                            " too large for varint buffer of size " +
                            std::to_string(data_.size()));
  }
  Codec::encode(value, output_iterator(extent));
  size_ = data_.size();
}

template <typename Codec, typename Container>
template <typename Integral>
void basic_varint<Codec, Container>::assign(Integral value,
                                            detail::dynamic_extent_t extent) {
  static_assert(std::is_signed<Integral>::value ==
                std::is_signed<Codec>::value);
  data_.clear();
  data_.resize(Codec::encode(value, output_iterator(extent)));
  size_ = data_.size();
}

template <typename Codec, typename Container>
template <typename T>
basic_varint<Codec, Container>::operator T() const {
  static_assert(
      std::is_integral<T>::value || std::is_same<std::string_view, T>::value,
      "unsupported cast");
  using tag = typename std::is_integral<T>::type;
  return get<T>(tag());
}

template <typename Codec, typename Container>
template <typename T>
T basic_varint<Codec, Container>::get(std::true_type) const {
  using InputIterator = decltype(std::begin(data_));
  auto begin = std::begin(data_);
  auto end = begin;
  std::advance(end, size_);
  return Codec::template decode<InputIterator, T>(begin, end);
}

template <typename Codec, typename Container>
template <typename T>
std::string_view basic_varint<Codec, Container>::get(std::false_type) const {
  return static_cast<std::string_view>(*this);
}

template <typename Codec, typename Container>
basic_varint<Codec, Container>::operator std::string_view() const {
  return std::string_view(data_.data(), size_);
}

template <typename Codec, typename ContainerL, typename ContainerR>
bool operator==(const basic_varint<Codec, ContainerL>& lhs,
                const basic_varint<Codec, ContainerR>& rhs) {
  return Codec::equal(static_cast<std::string_view>(lhs),
                      static_cast<std::string_view>(rhs));
}

template <typename Codec, typename ContainerL, typename ContainerR>
bool operator!=(const basic_varint<Codec, ContainerL>& lhs,
                const basic_varint<Codec, ContainerR>& rhs) {
  return !(lhs == rhs);
}

template <typename Codec, typename ContainerL, typename ContainerR>
bool operator<(const basic_varint<Codec, ContainerL>& lhs,
               const basic_varint<Codec, ContainerR>& rhs) {
  return Codec::less(static_cast<std::string_view>(lhs),
                     static_cast<std::string_view>(rhs));
}

template <typename Codec, typename ContainerL, typename ContainerR>
bool operator>(const basic_varint<Codec, ContainerL>& lhs,
               const basic_varint<Codec, ContainerR>& rhs) {
  return rhs < lhs;
}

template <typename Codec, typename ContainerL, typename ContainerR>
bool operator<=(const basic_varint<Codec, ContainerL>& lhs,
                const basic_varint<Codec, ContainerR>& rhs) {
  return !(lhs > rhs);
}

template <typename Codec, typename ContainerL, typename ContainerR>
bool operator>=(const basic_varint<Codec, ContainerL>& lhs,
                const basic_varint<Codec, ContainerR>& rhs) {
  return !(lhs < rhs);
}

template <typename Codec, typename Container>
std::istream& operator>>(std::istream& is, basic_varint<Codec, Container>& vi) {
  auto pos = is.tellg();
  using extent_type = typename detail::extent_type<Container>::type;
  Codec::copy(std::istreambuf_iterator<char>(is),
              std::istreambuf_iterator<char>(),
              vi.output_iterator(extent_type()));
  vi.size_ = is.tellg() - pos;
  return is;
}

template <typename Codec, typename Container>
std::ostream& operator<<(std::ostream& os, basic_varint<Codec, Container>& vi) {
  std::copy(std::begin(vi.data_), std::end(vi.data_),
            std::ostreambuf_iterator<char>(os));
  return os;
}

}  // namespace varint