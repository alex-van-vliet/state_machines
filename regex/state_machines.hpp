#pragma once

#include <cstddef>
#include <iostream>

#include "transitions.hpp"

namespace regex {
    template<typename FromState, typename ToState, typename Transition>
    struct transition_entry {
        using from_state = FromState;
        using to_state = ToState;
        using transition = Transition;
    };

    struct draw_transition_entry {
        template<size_t FromState, size_t ToState>
        void operator()(transition_entry<state<FromState>, state<ToState>, epsilon_transition>) const {
            std::cout << "    " << FromState << " -> " << ToState << ";\n";
        }

        template<size_t FromState, size_t ToState, char Value>
        void operator()(transition_entry<state<FromState>, state<ToState>, character_transition<Value>>) const {
            std::cout << "    " << FromState << " -> " << ToState << " [label=" << Value << "];\n";
        }
    };

    template<size_t Offset>
    struct offset_transition_entry {
        template<typename TransitionEntry>
        using type = transition_entry<state<Offset + TransitionEntry::from_state::id>,
                                      state<Offset + TransitionEntry::to_state::id>,
                                      typename TransitionEntry::transition>;
    };

    template<typename StateList>
    struct filter_transition_entry_from {
        template<typename TransitionEntry>
        static constexpr bool value = helpers::list_contains_v<StateList, typename TransitionEntry::from_state>;
    };

    template<typename StateList>
    struct filter_transition_entry_to {
        template<typename TransitionEntry>
        static constexpr bool value = helpers::list_contains_v<StateList, typename TransitionEntry::to_state>;
    };

    template<typename TransitionList>
    struct filter_transition_entry_transition {
        template<typename TransitionEntry>
        static constexpr bool value = helpers::list_contains_v<TransitionList, typename TransitionEntry::transition>;
    };

    struct map_transition_entry_from {
        template<typename TransitionEntry>
        using type = typename TransitionEntry::from_state;
    };

    struct map_transition_entry_to {
        template<typename TransitionEntry>
        using type = typename TransitionEntry::to_state;
    };

    struct map_transition_entry_transition {
        template<typename TransitionEntry>
        using type = typename TransitionEntry::transition;
    };

    template<std::size_t StateCount, typename InitState, typename FinalState, typename TransitionList>
    struct epsilon_nfa {
        static constexpr auto state_count = StateCount;
        using init_state = InitState;
        using final_state = FinalState;
        using transition_list = TransitionList;

        void draw() const {
            std::cout << "digraph {\n";
            std::cout << "    invisible [label=\"\", shape=none];\n";
            for (size_t i = 0; i < state_count; ++i) {
                std::cout << "    " << i << ";\n";
            }
            draw_final_state{}(final_state{});
            draw_init_state{}(init_state{});
            helpers::list_apply<transition_list, draw_transition_entry>{}();
            std::cout << "}" << std::endl;
        }
    };

    template<std::size_t StateCount, typename InitStateList, typename FinalStateList, typename TransitionList>
    struct nfa {
        static constexpr auto state_count = StateCount;
        using init_state_list = InitStateList;
        using final_state_list = FinalStateList;
        using transition_list = TransitionList;

        void draw() const {
            std::cout << "digraph {\n";
            std::cout << "    invisible [label=\"\", shape=none];\n";
            for (size_t i = 0; i < state_count; ++i) {
                std::cout << "    " << i << ";\n";
            }
            helpers::list_apply<final_state_list, draw_final_state>{}();
            helpers::list_apply<init_state_list, draw_init_state>{}();
            helpers::list_apply<transition_list, draw_transition_entry>{}();
            std::cout << "}" << std::endl;
        }
    };

    namespace detail {
        template<typename TransitionList, typename FromList>
        struct follow_epsilon_transitions {
            using transitions = helpers::list_filter_t<TransitionList, filter_transition_entry_from<FromList>>;
            using epsilon_transitions = helpers::list_filter_t<transitions, filter_transition_entry_transition<helpers::list<epsilon_transition>>>;
            using non_epsilon_transitions = helpers::list_filter_t<transitions, helpers::predicate_not<filter_transition_entry_transition<helpers::list<epsilon_transition>>>>;

            using next = follow_epsilon_transitions<TransitionList, helpers::list_map_t<epsilon_transitions, map_transition_entry_to>>;

            using transition_list_type = helpers::list_concat_t<non_epsilon_transitions, typename next::transition_list_type>;
            using state_list_type = helpers::list_concat_t<FromList, typename next::state_list_type>;
        };

        template<typename TransitionList>
        struct follow_epsilon_transitions<TransitionList, helpers::list<>> {
            using transition_list_type = helpers::list<>;
            using state_list_type = helpers::list<>;
        };

        template<typename TransitionList, typename ToList>
        struct reverse_epsilon_transitions {
            using transitions = helpers::list_filter_t<TransitionList, filter_transition_entry_to<ToList>>;
            using epsilon_transitions = helpers::list_filter_t<transitions, filter_transition_entry_transition<helpers::list<epsilon_transition>>>;
            using non_epsilon_transitions = helpers::list_filter_t<transitions, helpers::predicate_not<filter_transition_entry_transition<helpers::list<epsilon_transition>>>>;

            using next = reverse_epsilon_transitions<TransitionList, helpers::list_map_t<epsilon_transitions, map_transition_entry_from>>;

            using transition_list_type = helpers::list_concat_t<non_epsilon_transitions, typename next::transition_list_type>;
            using state_list_type = helpers::list_concat_t<ToList, typename next::state_list_type>;
        };

        template<typename TransitionList>
        struct reverse_epsilon_transitions<TransitionList, helpers::list<>> {
            using transition_list_type = helpers::list<>;
            using state_list_type = helpers::list<>;
        };

        template<size_t From>
        struct generate_transition_entry {
            template<typename ExistingEntry>
            using type = transition_entry<state<From>, typename ExistingEntry::to_state, typename ExistingEntry::transition>;
        };

        template<typename TransitionList, size_t... States>
        auto to_nfa_generate_transition_table(std::index_sequence<States...>)
                -> helpers::list_concat_t<helpers::list_map_t<typename follow_epsilon_transitions<TransitionList, helpers::list<state<States>>>::transition_list_type, generate_transition_entry<States>>...>;
    }// namespace detail

    // Converts an epsilon nfa to a nfa
    template<typename StateMachine>
    struct to_nfa {
        using type = nfa<
                StateMachine::state_count,
                typename detail::follow_epsilon_transitions<typename StateMachine::transition_list, helpers::list<typename StateMachine::init_state>>::state_list_type,
                typename detail::reverse_epsilon_transitions<typename StateMachine::transition_list, helpers::list<typename StateMachine::final_state>>::state_list_type,
                decltype(detail::to_nfa_generate_transition_table<typename StateMachine::transition_list>(std::make_index_sequence<StateMachine::state_count>{}))>;
    };

    template<typename StateMachine>
    using to_nfa_t = typename to_nfa<StateMachine>::type;
}// namespace regex
