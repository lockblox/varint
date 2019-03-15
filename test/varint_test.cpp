#include <gtest/gtest.h>
#include <varint/codecs/uleb128.h>
#include <varint/varint.h>
#include <array>

TEST(varint, compare) {
  using array = std::array<char, 2>;
  using varint_array = varint::uleb128<array>;
  using varint_string = varint::uleb128<std::string>;
  auto via = varint_array{128u};
  auto vis = varint_string{{-128, 1}};
  EXPECT_EQ(via, vis);
}

TEST(varint, assignment) {
  using array = std::array<char, 2>;
  using varint_array = varint::uleb128<array>;
  using inserter = std::back_insert_iterator<array>;
  varint_array via;
  via = 1233u;
  auto copy = via;
  EXPECT_EQ(via, copy);
  EXPECT_THROW(via = 65536u, std::out_of_range);
  EXPECT_THROW(static_cast<char>(via), std::overflow_error);
  EXPECT_EQ(static_cast<unsigned int>(via), 1233);
}

TEST(varint, input_stream) {
  std::string input_string = {-128, 1, 'h', 'e', 'l', 'l', 'o'};
  std::istringstream is{input_string};
  using varint_string = varint::uleb128<std::string>;
  varint_string vi;
  is >> vi;
  EXPECT_EQ(vi, static_cast<varint_string>(128u));
  auto hello = std::string{};
  std::copy(std::istreambuf_iterator<char>(is),
              std::istreambuf_iterator<char>(),
              std::back_inserter(hello));
  EXPECT_EQ("hello", hello);
}