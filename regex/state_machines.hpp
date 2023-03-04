#pragma once

#include <cstddef>
#include <iostream>

#include "helpers/map.hpp"
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

    template<std::size_t StateCount, typename InitState, typename FinalStateList, typename TransitionList>
    struct dfa {
        static constexpr auto state_count = StateCount;
        using init_state = InitState;
        using final_state_list = FinalStateList;
        using transition_list = TransitionList;

        void draw() const {
            std::cout << "digraph {\n";
            std::cout << "    invisible [label=\"\", shape=none];\n";
            for (size_t i = 0; i < state_count; ++i) {
                std::cout << "    " << i << ";\n";
            }
            helpers::list_apply<final_state_list, draw_final_state>{}();
            draw_init_state{}(init_state{});
            helpers::list_apply<transition_list, draw_transition_entry>{}();
            std::cout << "}" << std::endl;
        }
    };

    namespace detail {
        template<typename StateList>
        using state_list_to_group = helpers::list_unique_t<helpers::list_sort_t<StateList, state_to_id>, state_equals>;

        template<typename TransitionList>
        struct to_dfa_group_by_transition {};

        template<>
        struct to_dfa_group_by_transition<helpers::list<>> {
            using type = helpers::map<helpers::list<>>;
        };

        template<typename FromState, typename ToState, typename Transition, typename... TransitionEntries>
        struct to_dfa_group_by_transition<helpers::list<transition_entry<FromState, ToState, Transition>, TransitionEntries...>> {
            using next = to_dfa_group_by_transition<helpers::list<TransitionEntries...>>;

            using type = decltype(([] {
                using to_states = helpers::map_find_t<typename next::type, Transition>;
                if constexpr (std::is_same_v<to_states, helpers::map_not_found>) {
                    return helpers::map_insert_t<typename next::type, Transition, helpers::list<ToState>>{};
                } else {
                    return helpers::map_set_t<typename next::type, Transition, helpers::list_push_back_t<to_states, ToState>>{};
                }
            })());
        };

        struct to_dfa_map_state_list_to_group {
            template<typename MapEntry>
            using type = helpers::map_entry<typename MapEntry::key, state_list_to_group<typename MapEntry::value>>;
        };

        template<typename ConversionTable>
        struct to_dfa_group_exists {
            template<typename Group>
            static constexpr bool value = helpers::map_contains_v<ConversionTable, Group>;
        };

        template<typename ConversionTable, typename GroupList>
        struct to_dfa_add_groups_to_conversion_table {};

        template<typename ConversionTable, typename Group, typename... Groups>
        struct to_dfa_add_groups_to_conversion_table<ConversionTable, helpers::list<Group, Groups...>> {
            using type = typename to_dfa_add_groups_to_conversion_table<
                    helpers::map_insert_t<ConversionTable, Group, state<helpers::list_length_v<helpers::map_list_t<ConversionTable>>>>,
                    helpers::list<Groups...>>::type;
        };

        template<typename ConversionTable>
        struct to_dfa_add_groups_to_conversion_table<ConversionTable, helpers::list<>> {
            using type = ConversionTable;
        };

        template<typename ConversionTable, typename FromState>
        struct to_dfa_grouped_transitions_to_transition_list_mapper {
            template<typename MapEntry>
            using type = transition_entry<FromState, helpers::map_find_t<ConversionTable, typename MapEntry::value>, typename MapEntry::key>;
        };

        // The conversion table is a list<list<state>> where each inner list<state> is sorted
        // The remaining state lists is the part of the conversion table that has not been treated yet
        template<typename TransitionList, typename ConversionTable, typename RemainingGroupList>
        struct to_dfa_generate_conversion_table {};

        template<typename TransitionList, typename ConversionTable, typename CurrentGroup, typename... RemainingGroups>
        struct to_dfa_generate_conversion_table<TransitionList, ConversionTable, helpers::list<CurrentGroup, RemainingGroups...>> {
            using current_group_state = helpers::map_find_t<ConversionTable, CurrentGroup>;

            using transitions = helpers::list_filter_t<TransitionList, filter_transition_entry_from<CurrentGroup>>;

            using grouped_transitions = helpers::map<helpers::list_map_t<helpers::map_list_t<typename to_dfa_group_by_transition<transitions>::type>, to_dfa_map_state_list_to_group>>;

            using new_groups = helpers::list_filter_t<helpers::map_values_t<grouped_transitions>, helpers::predicate_not<to_dfa_group_exists<ConversionTable>>>;

            using new_conversion_table = typename detail::to_dfa_add_groups_to_conversion_table<ConversionTable, new_groups>::type;

            using next = to_dfa_generate_conversion_table<TransitionList,
                                                          new_conversion_table,
                                                          helpers::list_concat_t<
                                                                  helpers::list<RemainingGroups...>,
                                                                  new_groups>>;

            using type = typename next::type;

            using transition_list_type = helpers::list_concat_t<
                    helpers::list_map_t<helpers::map_list_t<grouped_transitions>, to_dfa_grouped_transitions_to_transition_list_mapper<new_conversion_table, current_group_state>>,
                    typename next::transition_list_type>;
            // Create the transitions
        };

        template<typename TransitionList, typename ConversionTable>
        struct to_dfa_generate_conversion_table<TransitionList, ConversionTable, helpers::list<>> {
            using type = ConversionTable;
            using transition_list_type = helpers::list<>;
        };

        template<typename FinalStateList>
        struct to_dfa_is_final {
            template<typename MapEntry>
            static constexpr bool value = !std::is_same_v<helpers::list_find_t<typename MapEntry::key, filter_state<FinalStateList>>, helpers::list_not_found>;
        };
    }// namespace detail

    // Converts a nfa to a dfa
    template<typename StateMachine>
    struct to_dfa {
        using conversion_table = detail::to_dfa_generate_conversion_table<
                typename StateMachine::transition_list,
                helpers::map<helpers::list<helpers::map_entry<detail::state_list_to_group<typename StateMachine::init_state_list>, state<0>>>>,
                helpers::list<detail::state_list_to_group<typename StateMachine::init_state_list>>>;

        using type = dfa<
                helpers::list_length_v<helpers::map_list_t<typename conversion_table::type>>,
                state<0>,
                helpers::map_values_t<helpers::map<helpers::list_filter_t<helpers::map_list_t<typename conversion_table::type>, detail::to_dfa_is_final<typename StateMachine::final_state_list>>>>,
                typename conversion_table::transition_list_type>;
    };

    template<typename StateMachine>
    using to_dfa_t = typename to_dfa<StateMachine>::type;

    template<typename StateMachine, helpers::string String, size_t Pos = 0, typename State = typename StateMachine::init_state>
    struct test {
        static constexpr bool value = ([]{
            if constexpr (Pos == String.size()) {
                return helpers::list_contains_v<typename StateMachine::final_state_list, State>;
            } else {
                using transition =
                        helpers::list_find_t<typename StateMachine::transition_list, helpers::predicate_and<filter_transition_entry_from<helpers::list<State>>,
                                                                                                            filter_transition_entry_transition<helpers::list<character_transition<String.at(Pos)>>>>>;
                if constexpr (std::is_same_v<transition, helpers::list_not_found>) {
                    return false;
                } else {
                    return test<StateMachine, String, Pos + 1, typename transition::to_state>::value;
                }
            }
        })();
    };

    template<typename StateMachine, helpers::string String>
    constexpr auto test_v = test<StateMachine, String>::value;
}// namespace regex
