#include "epsilon_nfa.hpp"
#include "transition_list.hpp"

namespace regex {
    template<typename SELF>
    struct Token;

    template<typename LEFT, typename RIGHT>
    struct Concatenation : Token<Concatenation<LEFT, RIGHT>> {
        using left = LEFT;
        using right = RIGHT;
    };

    template<typename LEFT, typename RIGHT>
    struct Union : Token<Union<LEFT, RIGHT>> {
        using left = LEFT;
        using right = RIGHT;
    };

    template<typename SELF>
    struct Token {
        using self = SELF;

        template<typename OTHER>
        constexpr auto operator&(const OTHER) const {
            return Concatenation<self, OTHER>{};
        }

        template<typename OTHER>
        constexpr auto operator|(const OTHER) const {
            return Union<self, OTHER>{};
        }
    };

    template<char C>
    struct Character : Token<Character<C>> {
        constexpr static char c = C;
    };

    template<char FROM, char TO>
    struct CharacterRange : Token<CharacterRange<FROM, TO>> {
        constexpr static char from = FROM;
        constexpr static char to = TO;
    };

    template<typename OPERAND>
    struct KleeneStar : Token<KleeneStar<OPERAND>> {
        using operand = OPERAND;
    };

    template<typename OPERAND>
    constexpr auto star(const OPERAND) {
        return KleeneStar<OPERAND>{};
    }

    template<typename T>
    struct ToStateMachine {};

    template<char C>
    struct ToStateMachine<Character<C>> {
        using state_machine = ::epsilon_nfa::StateMachine<2, 0, 1, ::transition_list::Node<::transition_list::Entry<::transitions::Character<C>, 0, 1>>>;
    };

    template<char FROM>
    struct ToStateMachine<CharacterRange<FROM, FROM>> {
        using state_machine = ::epsilon_nfa::StateMachine<2, 0, 1, ::transition_list::Node<::transition_list::Entry<::transitions::Character<FROM>, 0, 1>>>;
    };

    template<char FROM, char TO>
    struct ToStateMachine<CharacterRange<FROM, TO>> {
        using state_machine = ::epsilon_nfa::Union<
                ::epsilon_nfa::StateMachine<2, 0, 1, ::transition_list::Node<::transition_list::Entry<::transitions::Character<FROM>, 0, 1>>>,
                typename ToStateMachine<CharacterRange<FROM + 1, TO>>::state_machine>;
    };

    template<typename LEFT, typename RIGHT>
    struct ToStateMachine<Concatenation<LEFT, RIGHT>> {
        using state_machine = ::epsilon_nfa::Concatenation<typename ToStateMachine<LEFT>::state_machine, typename ToStateMachine<RIGHT>::state_machine>;
    };

    template<typename LEFT, typename RIGHT>
    struct ToStateMachine<Union<LEFT, RIGHT>> {
        using state_machine = ::epsilon_nfa::Union<typename ToStateMachine<LEFT>::state_machine, typename ToStateMachine<RIGHT>::state_machine>;
    };

    template<typename OPERAND>
    struct ToStateMachine<KleeneStar<OPERAND>> {
        using state_machine = ::epsilon_nfa::KleeneStar<typename ToStateMachine<OPERAND>::state_machine>;
    };

    template<typename T>
    using ToStateMachineT = typename ToStateMachine<T>::state_machine;

}// namespace regex
