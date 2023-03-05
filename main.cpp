#include <array>

#include "dfa.hpp"
#include "nfa.hpp"
#include "regex.hpp"
#include "runtime_nfa.hpp"
#include "string.hpp"

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

using regex::Character, regex::CharacterRange, regex::star;

// FIXME: code below is temporary just to be feature-compatible with the new version

template<size_t N>
    requires(N >= 1)
struct StringLiteral {
    std::array<char, N - 1> value{};

    constexpr StringLiteral(const char (&str)[N]) {
        std::copy_n(str, N - 1, value.begin());
    }
};

template<StringLiteral S, size_t I = 0>
    requires(S.value.size() > 0)
struct StringLiteralToUnions {
    static constexpr auto value = ([] {
        if constexpr (I == S.value.size() - 1) {
            return Character<S.value[0]>{};
        } else {
            return StringLiteralToUnions<S, I + 1>::value | Character<S.value[S.value.size() - 1 - I]>{};
        }
    })();
};

template<template<auto... Values> typename ResultType, typename ElementType, size_t N, std::array<ElementType, N> Array, size_t... Is>
auto as_pack_helper(std::integer_sequence<size_t, Is...>) -> ResultType<Array[Is]...>;

template<template<auto... Values> typename ResultType, std::array Array>
using as_pack_t = decltype(as_pack_helper<ResultType, typename decltype(Array)::value_type, Array.size(), Array>(std::make_index_sequence<Array.size()>{}));

template<StringLiteral S>
using as_string_t = as_pack_t<::string::String, S.value>;

template<StringLiteral S>
    requires(S.value.size() > 0)
constexpr auto operator""_c() {
    return StringLiteralToUnions<S>::value;
}

template<StringLiteral S>
    requires(S.value.size() == 3 && S.value[1] == '-')
constexpr auto operator""_r() {
    return CharacterRange<S.value[0], S.value[2]>{};
}

template<StringLiteral S>
constexpr auto operator""_s() {
    return as_string_t<S>{};
}

#ifdef STAGE1
using sm1 = regex::ToStateMachineT<decltype(("123456789"_c & star("0-9"_r)) | "0"_c)>;
#endif
#ifdef STAGE2
using sm2 = nfa::EpsilonRemovalT<sm1>;
#endif
#ifdef STAGE3
using sm3 = dfa::DFAConversionT<sm2>;
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
    static_assert(dfa::EvaluateV<sm3, decltype("1234567890"_s)>);
#endif
#ifdef STAGE5
    std::cout << "1234567890"
              << ": " << 1
              << " - " << runtime_nfa::ToRuntimeT<sm3>{}.test("1234567890")
              << " - " << dfa::EvaluateV<sm3, decltype("1234567890"_s)> << std::endl;
#endif
#ifdef STAGE6
#define TEST(sm, expected, s) std::cout << s << ": " << expected << " - " << runtime_nfa::ToRuntimeT<sm>{}.test(s) << " - " << dfa::EvaluateV<sm, decltype(s##_s)> << std::endl;
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
