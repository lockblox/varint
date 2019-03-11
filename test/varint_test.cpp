#include <gtest/gtest.h>
#include <varint/uleb128.h>
#include <varint/varint.h>

class encoding_test
    : public testing::TestWithParam<std::pair<std::size_t, std::string>> {};

class round_trip_test : public testing::TestWithParam<std::size_t> {};

TEST_P(encoding_test, encoding) {
  auto [input, expected] = GetParam();
  auto uleb128 = varint::uleb128();
  auto result = uleb128.encode(input);
  EXPECT_EQ(expected, result);
  EXPECT_EQ(result.size(), uleb128.encoded_size(input));
}

TEST_P(round_trip_test, round_trip) {
  auto input = GetParam();
  auto uleb128 = varint::uleb128();
  auto encoded = uleb128.encode(input);
  EXPECT_EQ(input, uleb128.decode(encoded));
}

INSTANTIATE_TEST_CASE_P(
    varint, encoding_test,
    ::testing::Values(std::pair{0, std::string{0}},
                      std::pair{127, std::string{127}},
                      std::pair{128, std::string{-128, 1}},
                      std::pair{255, std::string{-1, 1}},
                      std::pair{300, std::string{-84, 2}},
                      std::pair{16384, std::string{-128, -128, 1}}));

INSTANTIATE_TEST_CASE_P(
    varint, round_trip_test,
    ::testing::Values(0, 127, 128, 255, 256, 32767, 32768, 65535, 65536,
                      2147483647, 2147483648, 4294967295, 4294967296,
                      9223372036854775807, 9223372036854775808,
                      18446744073709551615));