#pragma once
#include <varint/codecs/uleb128.h>
#include <string>
#include <type_traits>

namespace varint {

struct dynamic_capacity_tag {};
struct static_capacity_tag {};

template <typename Container, typename = void>
class container_traits {
 public:
  using capacity_type = static_capacity_tag;
};

template <typename Container>
class container_traits<Container,
                       std::void_t<decltype(std::declval<Container>().push_back(
                           std::declval<typename Container::value_type>()))>> {
 public:
  using capacity_type = dynamic_capacity_tag;
};

/** Value type holding a varint-encoded integral value */
template <typename Codec, typename Container>
class varint {
 public:
  static_assert(std::is_class<Container>::value);
  static_assert(std::is_same<typename Container::value_type, char>::value);

  /** Default constructor */
  varint();

  /** Returns an iterator to the beginning of the encoding */
  auto cbegin() const { return std::begin(data_); }

  /** Returns an iterator to the end of the encoding */
  auto cend() const { return std::end(data_); }

  /** Read from a stream */
  template <typename Codec, typename Container>
  friend std::istream& operator>>(std::istream& is,
                                  varint<Codec, Container>& vi);

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
  auto output_iterator(dynamic_capacity_tag);
  auto output_iterator(static_capacity_tag);

  template <typename Integral>
  void assign(Integral value, static_capacity_tag capacity_tag);

  template <typename Integral>
  void assign(Integral value, dynamic_capacity_tag capacity_tag);

  Container data_;
};

/** Compare varints with the same codec */
template <typename Codec, typename LhsContainer, typename RhsContainer>
bool operator==(const varint<Codec, LhsContainer>& lhs,
                const varint<Codec, RhsContainer>& rhs);

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
  assign(value, container_traits<Container>::capacity_type{});
  return *this;
}

template <typename Codec, typename Container>
auto varint<Codec, Container>::output_iterator(dynamic_capacity_tag) {
  return std::back_inserter(data_);
}

template <typename Codec, typename Container>
auto varint<Codec, Container>::output_iterator(static_capacity_tag) {
  return std::begin(data_);
}

template <typename Codec, typename Container>
template <typename Integral>
void varint<Codec, Container>::assign(Integral value,
                                      static_capacity_tag capacity_tag) {
  static_assert(std::is_signed<Integral>::value ==
                std::is_signed<Codec>::value);
  if (size() < Codec::size(value)) {
    throw std::out_of_range("value " + std::to_string(value) +
                            " too large for varint buffer of size " +
                            std::to_string(size()));
  }
  Codec::encode(value, output_iterator(capacity_tag));
}

template <typename Codec, typename Container>
template <typename Integral>
void varint<Codec, Container>::assign(Integral value,
                                      dynamic_capacity_tag capacity_tag) {
  static_assert(std::is_signed<Integral>::value ==
                std::is_signed<Codec>::value);
  data_.clear();
  data_.resize(Codec::encode(value, output_iterator(capacity_tag)));
}

template <typename Codec, typename Container>
template <typename Integral>
varint<Codec, Container>::operator Integral() const {
  static_assert(std::is_integral<Integral>::value,
                "cannot cast to non-integral type");
  using InputIterator = decltype(std::begin(data_));
  return Codec::decode<InputIterator, Integral>(std::begin(data_),
                                                std::end(data_));
}

template <typename Codec, typename LhsContainer, typename RhsContainer>
bool operator==(const varint<Codec, LhsContainer>& lhs,
                const varint<Codec, RhsContainer>& rhs) {
  return lhs.size() == rhs.size() &&
         std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
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
  Codec::copy(std::istreambuf_iterator<char>(is),
              std::istreambuf_iterator<char>(),
              vi.output_iterator(container_traits<Container>::capacity_type{}));
  return is;
}

}  // namespace varint