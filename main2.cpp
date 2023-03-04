#include "helpers/string.hpp"
#include "regex/tokens.hpp"

using namespace helpers::literals;
using namespace regex::literals;
using sm1 = regex::to_state_machine_t<decltype(("123456789"_c & regex::star("0-9"_r)) | "0"_c)>;
using sm2 = regex::to_nfa_t<sm1>;
using sm3 = regex::to_dfa_t<sm2>;

int main() {
#define TEST(sm, expected, s) std::cout << s << ": " << expected << " - " << regex::runtime<sm>{}.test(s) << " - " << regex::test_v<sm, s> << std::endl;
    TEST(sm3, 0, "")
    TEST(sm3, 1, "0")
    TEST(sm3, 1, "1")
    TEST(sm3, 0, "01")
    TEST(sm3, 1, "10")
    TEST(sm3, 1, "1234567890")
    TEST(sm3, 1, "42")
    TEST(sm3, 0, "042")

    std::cout << "1234567890"
              << ": " << 1
              << " - " << regex::runtime<sm3>{}.test("1234567890")
              << " - " << regex::test_v<sm3, "1234567890"> << std::endl;
}
