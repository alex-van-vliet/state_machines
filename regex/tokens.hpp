#pragma once

#include "helpers/list.hpp"
#include "helpers/string.hpp"
#include "state.hpp"
#include "state_machines.hpp"
#include "transitions.hpp"

namespace regex {
    template<typename LeftOperand, typename RightOperand>
    class Union;

    template<typename LeftOperand, typename RightOperand>
    class Concatenation;

    template<typename Self>
    class Token {
        using self = Self;

    public:
        template<typename Other>
        constexpr auto operator&(Other) const {
            return Concatenation<self, Other>{};
        }


        template<typename Other>
        constexpr auto operator|(Other) const {
            return Union<self, Other>{};
        };
    };

    template<char Value>
    class Character : public Token<Character<Value>> {
        static constexpr auto value = Value;
    };

    template<char FromValue, char ToValue>
    class CharacterRange : public Token<CharacterRange<FromValue, ToValue>> {
        static constexpr auto from_value = FromValue;
        static constexpr auto to_value = ToValue;
    };

    template<typename LeftOperand, typename RightOperand>
    class Union : public Token<Union<LeftOperand, RightOperand>> {
        using left_operand = LeftOperand;
        using right_operand = RightOperand;
    };

    template<typename LeftOperand, typename RightOperand>
    class Concatenation : public Token<Concatenation<LeftOperand, RightOperand>> {
        using left_operand = LeftOperand;
        using right_operand = RightOperand;
    };

    template<typename Operand>
    class Star : public Token<Star<Operand>> {
        using operand = Operand;
    };

    template<typename Operand>
    auto star(Operand) -> Star<Operand>;

    namespace literals {
        namespace detail {
            template<helpers::string Str, size_t I = 0>
                requires(Str.size() > 0)
            struct string_to_union {
                static constexpr auto value = ([] {
                    if constexpr (I == Str.size() - 1) {
                        return Character<Str.at(0)>{};
                    } else {
                        return string_to_union<Str, I + 1>::value | Character<Str.at(Str.size() - 1 - I)>{};
                    }
                })();
            };
        }// namespace detail

        template<helpers::string Str>
            requires(Str.size() > 0)
        constexpr auto operator""_c() {
            return detail::string_to_union<Str>::value;
        }

        template<helpers::string Str>
            requires(Str.size() == 3 && Str.at(1) == '-')
        constexpr auto operator""_r() {
            return CharacterRange<Str.at(0), Str.at(2)>{};
        }
    }// namespace literals

    template<typename Token>
    struct to_state_machine {};

    template<char Value>
    struct to_state_machine<Character<Value>> {
        using type = epsilon_nfa<2, state<0>, state<1>,
                                 helpers::list<transition_entry<state<0>, state<1>, character_transition<Value>>>>;
    };

    namespace detail {
        template<char FromValue, char... Values>
        auto to_state_machine_character_range(std::integer_sequence<char, Values...>)
                -> helpers::list<transition_entry<state<0>, state<1>, character_transition<FromValue + Values>>...>;
    }

    template<char FromValue, char ToValue>
    struct to_state_machine<CharacterRange<FromValue, ToValue>> {
        using type = typename to_state_machine<Union<Character<FromValue>, CharacterRange<FromValue + 1, ToValue>>>::type;
    };

    template<char Value>
    struct to_state_machine<CharacterRange<Value, Value>> {
        using type = typename to_state_machine<Character<Value>>::type;
    };

    template<typename LeftOperand, typename RightOperand>
    struct to_state_machine<Union<LeftOperand, RightOperand>> {
        using left_state_machine = typename to_state_machine<LeftOperand>::type;
        using right_state_machine = typename to_state_machine<RightOperand>::type;

        static constexpr auto state_count = left_state_machine::state_count + right_state_machine::state_count + 2;
        static constexpr auto init_state = left_state_machine::state_count + right_state_machine::state_count;
        static constexpr auto final_state = left_state_machine::state_count + right_state_machine::state_count + 1;

        using type = epsilon_nfa<
                state_count,
                state<init_state>,
                state<final_state>,
                helpers::list_concat_t<
                        typename left_state_machine::transition_list,
                        helpers::list_map_t<typename right_state_machine::transition_list, offset_transition_entry<left_state_machine::state_count>>,
                        helpers::list<
                                transition_entry<state<init_state>, state<left_state_machine::init_state::id>, epsilon_transition>,
                                transition_entry<state<init_state>, state<right_state_machine::init_state::id + left_state_machine::state_count>, epsilon_transition>,
                                transition_entry<state<left_state_machine::final_state::id>, state<final_state>, epsilon_transition>,
                                transition_entry<state<right_state_machine::final_state::id + left_state_machine::state_count>, state<final_state>, epsilon_transition>>>>;
    };

    template<typename LeftOperand, typename RightOperand>
    struct to_state_machine<Concatenation<LeftOperand, RightOperand>> {
        using left_state_machine = typename to_state_machine<LeftOperand>::type;
        using right_state_machine = typename to_state_machine<RightOperand>::type;

        static constexpr auto state_count = left_state_machine::state_count + right_state_machine::state_count;
        static constexpr auto init_state = left_state_machine::init_state::id;
        static constexpr auto final_state = left_state_machine::state_count + right_state_machine::final_state::id;

        using type = epsilon_nfa<
                state_count,
                state<init_state>,
                state<final_state>,
                helpers::list_concat_t<
                        typename left_state_machine::transition_list,
                        helpers::list_map_t<typename right_state_machine::transition_list, offset_transition_entry<left_state_machine::state_count>>,
                        helpers::list<
                                transition_entry<state<left_state_machine::final_state::id>, state<right_state_machine::init_state::id + left_state_machine::state_count>, epsilon_transition>>>>;
    };

    template<typename Operand>
    struct to_state_machine<Star<Operand>> {
        using state_machine = typename to_state_machine<Operand>::type;

        static constexpr auto state_count = state_machine::state_count + 2;
        static constexpr auto init_state = state_machine::state_count;
        static constexpr auto final_state = state_machine::state_count + 1;

        using type = epsilon_nfa<
                state_count,
                state<init_state>,
                state<final_state>,
                helpers::list_concat_t<
                        typename state_machine::transition_list,
                        helpers::list<
                                transition_entry<state<init_state>, state<state_machine::init_state::id>, epsilon_transition>,
                                transition_entry<state<state_machine::final_state::id>, state<final_state>, epsilon_transition>,
                                transition_entry<state<init_state>, state<final_state>, epsilon_transition>,
                                transition_entry<state<state_machine::final_state::id>, state<state_machine::init_state::id>, epsilon_transition>>>>;
    };

    template<typename Token>
    using to_state_machine_t = typename to_state_machine<Token>::type;
}// namespace regex
