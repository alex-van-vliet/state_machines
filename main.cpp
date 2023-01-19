#include "dfa.hpp"
#include "epsilon_nfa.hpp"
#include "nfa.hpp"
#include "runtime_nfa.hpp"
#include "string.hpp"
#include "transition_list.hpp"
#include "transitions.hpp"

using epsilon_nfa::StateMachine, epsilon_nfa::Concatenation, epsilon_nfa::Union, epsilon_nfa::KleeneStar;
using transition_list::Entry, transition_list::Node;
using transitions::Character, transitions::Epsilon;

using sm1 = KleeneStar<Union<
        StateMachine<3, 0, 2, Node<Entry<Character<'a'>, 0, 1>, Node<Entry<Character<'b'>, 1, 2>>>>,
        StateMachine<2, 0, 1, Node<Entry<Character<'c'>, 0, 1>>>>>;
using sm2 = nfa::EpsilonRemovalT<sm1>;
using sm3 = dfa::DFAConversionT<sm2>;

int main() {
    sm1::draw();
    sm2::draw();
    sm3::draw();

#define TEST(sm, s, ...) std::cout << s << ": " << runtime_nfa::ToRuntimeT<sm>{}.test(s) << " - " << dfa::EvaluateV<sm, ::string::String<__VA_ARGS__>> << std::endl;
    TEST(sm3, "")
    TEST(sm3, "ab", 'a', 'b')
    TEST(sm3, "c", 'c')
    TEST(sm3, "abc", 'a', 'b', 'c')
    TEST(sm3, "abccc", 'a', 'b', 'c', 'c', 'c')
    TEST(sm3, "b", 'b')
    TEST(sm3, "a", 'a')

    return 0;
}
