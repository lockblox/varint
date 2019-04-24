#pragma once
#include <type_traits>

namespace varint {
namespace detail {

struct dynamic_extent_t {};
struct static_extent_t {};

template <typename Container, typename = void>
class extent_type {
 public:
  using type = static_extent_t;
};

template <typename Container>
class extent_type<Container,
                  std::void_t<decltype(std::declval<Container>().push_back(
                      std::declval<typename Container::value_type>()))>> {
 public:
  using type = dynamic_extent_t;
};

}  // namespace detail
}  // namespace varint