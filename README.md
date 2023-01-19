# C++ Compile-Time Regexes with Templates

_Disclaimer: this is a project to learn more about metaprogramming in C++._

## Functionalities

The implementation supports single characters, ranges of characters, and their combination using the concatenation,
union and Kleene star operators. For example, it can recognize `([1-9][0-9]*)|0`.

The expression is inputted using the classes in `regex.hpp`, which can be converted into an epsilon-NFA using
`regex::ToStateMachineT` (implementing Thompson's construction). Then, it can be converted into a NFA using
`nfa::EpsilonRemovalT` (implementing the epsilon elimination algorithm). Finally, it can be converted into a
DFA using `dfa::DFAConversionT` (implementing the determinisation algorithm). A word can be tested against the
resulting DFA at compile time using `dfa::EvalulateV` or at runtime using `runtime_nfa::ToRuntimeT`.

For example, the regex `([1-9][0-9]*)|0` can be tested with:

```c++
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
```

Where `sm1` will be the epsilon-NFA, `sm2` the NFA, and `sm3` the dfa.

State machines can also produce a visual graph of themselves using their draw function which outputs in dot format.
