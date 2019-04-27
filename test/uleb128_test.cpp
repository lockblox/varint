#include <gtest/gtest.h>
#include <varint/codecs/uleb128.h>
#include <array>

class encoding_test
    : public testing::TestWithParam<std::pair<std::size_t, std::string>> {};

class round_trip_test : public testing::TestWithParam<std::size_t> {};

TEST_P(encoding_test, encoding) {
  auto [input, expected] = GetParam();
  auto result = std::string{};
  auto size =
      varint::codecs::uleb128::encode(input, std::back_inserter(result));
  EXPECT_EQ(expected.size(), size);
  EXPECT_TRUE(std::equal(result.begin(), result.end(), expected.begin()));
  EXPECT_EQ(result.size(), varint::codecs::uleb128::size(input));
  EXPECT_EQ(result, result);
}

TEST_P(round_trip_test, round_trip) {
  using uleb128 = varint::codecs::uleb128;
  auto input = GetParam();
  auto encoded = std::string{};
  uleb128::encode(input, std::back_inserter(encoded));
  auto decoded = uleb128::decode(encoded.begin(), encoded.end());
  EXPECT_EQ(input, decoded);
  try {
    uleb128::decode(encoded.end(), encoded.end());
    FAIL();
  } catch (std::out_of_range&) {
  }
}

INSTANTIATE_TEST_CASE_P(
    varint, encoding_test,
    ::testing::Values(std::pair{0, std::string{0}},
                      std::pair{127, std::string{127}},
                      std::pair{128, std::string{-128, 1}},
                      std::pair{255, std::string{-1, 1}},
                      std::pair{300, std::string{-84, 2}},
                      std::pair{16384, std::string{-128, -128, 1}}), );

INSTANTIATE_TEST_CASE_P(varint, round_trip_test,
                        ::testing::Values(0, 127, 128, 255, 256, 32767, 32768,
                                          65535, 65536, 2147483647, 2147483648,
                                          4294967295, 4294967296,
                                          9223372036854775807,
                                          9223372036854775808u,
                                          18446744073709551615u), );