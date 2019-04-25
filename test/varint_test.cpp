#include <gtest/gtest.h>
#include <varint/codecs/uleb128.h>
#include <varint/varint.h>
#include <array>
#include <functional>

TEST(varint, compare) {
  using array = std::array<char, 2>;
  using varint_array = varint::uleb128<array>;
  using varint_string = varint::uleb128<std::string>;
  auto via = varint_array{128u};
  auto vis = varint_string{{-128, 1}};
  EXPECT_EQ(via, vis);
  via = 117u;
  EXPECT_LT(via, vis);
  EXPECT_GE(vis, via);
  EXPECT_LE(via, vis);
  EXPECT_NE(via, vis);
  vis = 11658u;
  EXPECT_GT(vis, via);
  EXPECT_TRUE(std::less<>{}(via, vis));
}

TEST(varint, assignment) {
  using array = std::array<char, 2>;
  using varint_array = varint::uleb128<array>;
  varint_array via;
  via = 1233u;
  auto copy = via;
  EXPECT_EQ(via, copy);
  EXPECT_THROW(via = 65536u, std::out_of_range);
  EXPECT_THROW((void)(static_cast<char>(via)), std::overflow_error);
  EXPECT_EQ(static_cast<unsigned int>(via), 1233u);
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
            std::istreambuf_iterator<char>(), std::back_inserter(hello));
  EXPECT_EQ("hello", hello);
}

TEST(varint, output_stream) {
  std::ostringstream os;
  using varint_string = varint::uleb128<std::string>;
  auto vis = varint_string{16384u};
  os << "hello";
  os << vis;
  os << "world";
  auto expected = std::string{'h', 'e', 'l', 'l', 'o', -128, -128,
                              1,   'w', 'o', 'r', 'l', 'd'};
  EXPECT_EQ(expected, os.str());
}