#include "dfa.hpp"
#include "nfa.hpp"
#include "regex.hpp"
#include "runtime_nfa.hpp"
#include "string.hpp"

using regex::Character, regex::CharacterRange, regex::star;

constexpr auto non_null_digit = Character<'1'>{} | Character<'2'>{} | Character<'3'>{} | Character<'4'>{} | Character<'5'>{} | Character<'6'>{} | Character<'7'>{} | Character<'8'>{} | Character<'9'>{};
constexpr auto digit = CharacterRange<'0', '9'>{};
using sm1 = regex::ToStateMachineT<decltype((non_null_digit & star(digit)) | Character<'0'>{})>;
using sm2 = nfa::EpsilonRemovalT<sm1>;
using sm3 = dfa::DFAConversionT<sm2>;

int main() {
    sm1::draw();
    sm2::draw();
    sm3::draw();

#define TEST(sm, s, ...) std::cout << s << ": " << runtime_nfa::ToRuntimeT<sm>{}.test(s) << " - " << dfa::EvaluateV<sm, ::string::String<__VA_ARGS__>> << std::endl;
    TEST(sm3, "")
    TEST(sm3, "0", '0')
    TEST(sm3, "1", '1')
    TEST(sm3, "01", '0', '1')
    TEST(sm3, "10", '1', '0')
    TEST(sm3, "1234567890", '1', '2', '3', '4', '5', '6', '7', '8', '9', '0')
    TEST(sm3, "42", '4', '2')
    TEST(sm3, "042", '0', '4', '2')

    return 0;
}
