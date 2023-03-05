#include "helpers/string.hpp"
#include "regex/tokens.hpp"

#ifdef STAGE6
#define STAGE5
#endif
#ifdef STAGE5
#define STAGE4
#endif
#ifdef STAGE4
#define STAGE3
#endif
#ifdef STAGE3
#define STAGE2
#endif
#ifdef STAGE2
#define STAGE1
#endif

using namespace helpers::literals;
using namespace regex::literals;
#ifdef STAGE1
using sm1 = regex::to_state_machine_t<decltype(("123456789"_c & regex::star("0-9"_r)) | "0"_c)>;
#endif
#ifdef STAGE2
using sm2 = regex::to_nfa_t<sm1>;
#endif
#ifdef STAGE3
using sm3 = regex::to_dfa_t<sm2>;
#endif

int main() {
#ifdef STAGE1
    static_assert(!std::is_same_v<sm1, void>);
#endif
#ifdef STAGE2
    static_assert(!std::is_same_v<sm2, void>);
#endif
#ifdef STAGE3
    static_assert(!std::is_same_v<sm3, void>);
#endif
#ifdef STAGE4
    static_assert(regex::test_v<sm3, "1234567890">);
#endif
#ifdef STAGE5
    std::cout << "1234567890"
              << ": " << 1
              << " - " << regex::runtime<sm3>{}.test("1234567890")
              << " - " << regex::test_v<sm3, "1234567890"> << std::endl;
#endif
#ifdef STAGE6
#define TEST(sm, expected, s) std::cout << s << ": " << expected << " - " << regex::runtime<sm>{}.test(s) << " - " << regex::test_v<sm, s> << std::endl;
    TEST(sm3, 0, "")
    TEST(sm3, 1, "0")
    TEST(sm3, 1, "1")
    TEST(sm3, 0, "01")
    TEST(sm3, 1, "10")
    TEST(sm3, 1, "1234567890")
    TEST(sm3, 1, "42")
    TEST(sm3, 0, "042")
#endif
}
