#include <varint/codecs/uleb128.h>

namespace varint {
namespace codecs {

bool uleb128::less(std::string_view lhs, std::string_view rhs) {
  return lhs.size() < rhs.size() ||
         std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                      rhs.end(), [](auto x, auto y) {
                                        return static_cast<unsigned char>(x) <
                                               static_cast<unsigned char>(y);
                                      });
}

bool uleb128::equal(std::string_view lhs, std::string_view rhs) {
  return lhs.size() == rhs.size() &&
         std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

}  // namespace codecs
}  // namespace varint