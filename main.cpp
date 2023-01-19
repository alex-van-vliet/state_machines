#include "dfa.hpp"
#include "epsilon_nfa.hpp"
#include "nfa.hpp"
#include "runtime_nfa.hpp"
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

    runtime_nfa::ToRuntimeT<sm3> sm4;
#define TEST(sm, s) std::cout << s << ": " << sm4.test(s) << std::endl;
    TEST(sm4, "")
    TEST(sm4, "ab")
    TEST(sm4, "c")
    TEST(sm4, "abc")
    TEST(sm4, "abccc")
    TEST(sm4, "b")
    TEST(sm4, "a")

    return 0;
}
