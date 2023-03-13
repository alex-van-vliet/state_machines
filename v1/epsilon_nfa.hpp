#pragma once

#include "states.hpp"
#include "transition_list.hpp"
#include "transitions.hpp"
#include "types.hpp"
#include <iostream>

namespace epsilon_nfa {
    template<size_t STATES_COUNT, size_t INIT_STATE, size_t FINAL_STATE, typename TRANSITION_LIST>
    struct StateMachine {
        constexpr static size_t states_count = STATES_COUNT;
        constexpr static size_t init_state = INIT_STATE;
        constexpr static size_t final_state = FINAL_STATE;
        using transition_list = TRANSITION_LIST;

        static void draw() {
            std::cout << "digraph {\n";
            std::cout << "    invisible [label=\"\", shape=none];\n";
            for (size_t i = 0; i < states_count; ++i) {
                std::cout << "    " << i << ";\n";
            }
            ::states::DrawFinal{}.template operator()<FINAL_STATE>();
            ::states::DrawInit{}.template operator()<INIT_STATE>();
            ::transition_list::ForEach<transition_list, ::transitions::Draw>{}();
            std::cout << "}" << std::endl;
        }
    };


    template<size_t STATES_COUNT_1, typename TRANSITION_LIST_1, typename TRANSITION_LIST_2>
    struct Join {
        using transition_list = ::transition_list::Node<typename TRANSITION_LIST_1::entry,
                                                        typename Join<STATES_COUNT_1, typename TRANSITION_LIST_1::next, TRANSITION_LIST_2>::transition_list>;
    };

    template<size_t STATES_COUNT_1, typename TRANSITION_LIST_2>
    struct Join<STATES_COUNT_1, ::transition_list::End, TRANSITION_LIST_2> {
        using transition_list = ::transition_list::Node<::transition_list::Entry<typename TRANSITION_LIST_2::entry::transition, TRANSITION_LIST_2::entry::from + STATES_COUNT_1, TRANSITION_LIST_2::entry::to + STATES_COUNT_1>,
                                                        typename Join<STATES_COUNT_1, ::transition_list::End, typename TRANSITION_LIST_2::next>::transition_list>;
    };

    template<size_t STATES_COUNT_1>
    struct Join<STATES_COUNT_1, ::transition_list::End, ::transition_list::End> {
        using transition_list = ::transition_list::End;
    };

    template<size_t STATES_COUNT_1, typename TRANSITION_LIST_1, typename TRANSITION_LIST_2>
    using JoinT = typename Join<STATES_COUNT_1, TRANSITION_LIST_1, TRANSITION_LIST_2>::transition_list;

    template<typename STATE_MACHINE_1, typename STATE_MACHINE_2>
    using Concatenation = StateMachine<STATE_MACHINE_1::states_count + STATE_MACHINE_2::states_count,
                                       STATE_MACHINE_1::init_state, STATE_MACHINE_2::final_state + STATE_MACHINE_1::states_count,
                                       ::transition_list::Node<::transition_list::Entry<::transitions::Epsilon, STATE_MACHINE_1::final_state, STATE_MACHINE_2::init_state + STATE_MACHINE_1::states_count>, JoinT<STATE_MACHINE_1::states_count, typename STATE_MACHINE_1::transition_list, typename STATE_MACHINE_2::transition_list>>>;

    template<typename STATE_MACHINE_1, typename STATE_MACHINE_2>
    using Union = StateMachine<STATE_MACHINE_1::states_count + STATE_MACHINE_2::states_count + 2,
                               STATE_MACHINE_1::states_count + STATE_MACHINE_2::states_count, STATE_MACHINE_1::states_count + STATE_MACHINE_2::states_count + 1,
                               ::transition_list::Node<::transition_list::Entry<::transitions::Epsilon, STATE_MACHINE_1::states_count + STATE_MACHINE_2::states_count, STATE_MACHINE_1::init_state>,
                                                       ::transition_list::Node<::transition_list::Entry<::transitions::Epsilon, STATE_MACHINE_1::states_count + STATE_MACHINE_2::states_count, STATE_MACHINE_2::init_state + STATE_MACHINE_1::states_count>,
                                                                               ::transition_list::Node<::transition_list::Entry<::transitions::Epsilon, STATE_MACHINE_1::final_state, STATE_MACHINE_1::states_count + STATE_MACHINE_2::states_count + 1>,
                                                                                                       ::transition_list::Node<::transition_list::Entry<::transitions::Epsilon, STATE_MACHINE_2::final_state + STATE_MACHINE_1::states_count, STATE_MACHINE_1::states_count + STATE_MACHINE_2::states_count + 1>,
                                                                                                                               JoinT<STATE_MACHINE_1::states_count, typename STATE_MACHINE_1::transition_list, typename STATE_MACHINE_2::transition_list>>>>>>;

    template<typename STATE_MACHINE>
    using KleeneStar = StateMachine<STATE_MACHINE::states_count + 2,
                                    STATE_MACHINE::states_count, STATE_MACHINE::states_count + 1,
                                    ::transition_list::Node<::transition_list::Entry<::transitions::Epsilon, STATE_MACHINE::states_count, STATE_MACHINE::states_count + 1>,
                                                            ::transition_list::Node<::transition_list::Entry<::transitions::Epsilon, STATE_MACHINE::states_count, STATE_MACHINE::init_state>,
                                                                                    ::transition_list::Node<::transition_list::Entry<::transitions::Epsilon, STATE_MACHINE::final_state, STATE_MACHINE::states_count + 1>,
                                                                                                            ::transition_list::Node<::transition_list::Entry<::transitions::Epsilon, STATE_MACHINE::final_state, STATE_MACHINE::init_state>,
                                                                                                                                    typename STATE_MACHINE::transition_list>>>>>;
}// namespace epsilon_nfa
