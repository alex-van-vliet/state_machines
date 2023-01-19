#pragma once

#include "state_list.hpp"
#include "states.hpp"
#include "transition_list.hpp"
#include "transitions.hpp"
#include "types.hpp"
#include <iostream>

namespace nfa {
    template<size_t STATES_COUNT, typename INIT_STATE_LIST, typename FINAL_STATE_LIST, typename TRANSITION_LIST>
    struct StateMachine {
        constexpr static size_t states_count = STATES_COUNT;
        using init_state_list = INIT_STATE_LIST;
        using final_state_list = FINAL_STATE_LIST;
        using transition_list = TRANSITION_LIST;

        static void draw() {
            std::cout << "digraph {\n";
            std::cout << "    invisible [label=\"\", shape=none];\n";
            for (size_t i = 0; i < states_count; ++i) {
                std::cout << "    " << i << ";\n";
            }
            ::state_list::ForEach<final_state_list, ::states::DrawFinal>{}();
            ::state_list::ForEach<init_state_list, ::states::DrawInit>{}();
            ::transition_list::ForEach<transition_list, ::transitions::Draw>{}();
            std::cout << "}" << std::endl;
        }
    };

    template<typename TRANSITION_LIST>
    struct RemoveEpsilonTransitions {};

    template<size_t FROM, size_t TO, typename NEXT>
    struct RemoveEpsilonTransitions<::transition_list::Node<::transition_list::Entry<::transitions::Epsilon, FROM, TO>, NEXT>> {
        using transition_list = typename RemoveEpsilonTransitions<NEXT>::transition_list;
    };

    template<typename TRANSITION_ENTRY, typename NEXT>
    struct RemoveEpsilonTransitions<::transition_list::Node<TRANSITION_ENTRY, NEXT>> {
        using transition_list = ::transition_list::Node<TRANSITION_ENTRY, typename RemoveEpsilonTransitions<NEXT>::transition_list>;
    };

    template<>
    struct RemoveEpsilonTransitions<::transition_list::End> {
        using transition_list = ::transition_list::End;
    };

    template<typename TRANSITION_LIST>
    using RemoveEpsilonTransitionsT = typename RemoveEpsilonTransitions<TRANSITION_LIST>::transition_list;

    template<typename TRANSITION_LIST>
    struct OnlyEpsilonTransitions {};

    template<size_t FROM, size_t TO, typename NEXT>
    struct OnlyEpsilonTransitions<::transition_list::Node<::transition_list::Entry<::transitions::Epsilon, FROM, TO>, NEXT>> {
        using transition_list = ::transition_list::Node<::transition_list::Entry<::transitions::Epsilon, FROM, TO>, typename OnlyEpsilonTransitions<NEXT>::transition_list>;
    };

    template<typename TRANSITION_ENTRY, typename NEXT>
    struct OnlyEpsilonTransitions<::transition_list::Node<TRANSITION_ENTRY, NEXT>> {
        using transition_list = typename OnlyEpsilonTransitions<NEXT>::transition_list;
    };

    template<>
    struct OnlyEpsilonTransitions<::transition_list::End> {
        using transition_list = ::transition_list::End;
    };

    template<typename TRANSITION_LIST>
    using OnlyEpsilonTransitionsT = typename OnlyEpsilonTransitions<TRANSITION_LIST>::transition_list;

    template<typename SOURCES, typename TRANSITION_LIST>
    struct FollowEpsilonTransitionsFromSeveralStates;

    template<size_t SOURCE, typename TRANSITION_LIST>
    struct FollowEpsilonTransitionsFromOneState {
        using nexts = ::transition_list::FindFromT<SOURCE, TRANSITION_LIST>;
        using epsilon_transitions = OnlyEpsilonTransitionsT<nexts>;
        using non_epsilon_transitions = RemoveEpsilonTransitionsT<nexts>;

        using next_results = FollowEpsilonTransitionsFromSeveralStates<::transition_list::GetTosT<epsilon_transitions>, TRANSITION_LIST>;

        using state_list = ::state_list::ConcatenateT<
                ::transition_list::GetTosT<epsilon_transitions>,
                typename next_results::state_list>;

        using transition_list = ::transition_list::ConcatenateT<
                non_epsilon_transitions,
                typename next_results::transition_list>;
    };

    template<typename SOURCES, typename TRANSITION_LIST>
    struct FollowEpsilonTransitionsFromSeveralStates {
        using follow_current = FollowEpsilonTransitionsFromOneState<SOURCES::state, TRANSITION_LIST>;

        using next_results = FollowEpsilonTransitionsFromSeveralStates<typename SOURCES::next, TRANSITION_LIST>;

        using state_list = ::state_list::ConcatenateT<
                typename follow_current::state_list,
                typename next_results::state_list>;

        using transition_list = ::transition_list::ConcatenateT<
                typename follow_current::transition_list,
                typename next_results::transition_list>;
    };

    template<typename TRANSITION_LIST>
    struct FollowEpsilonTransitionsFromSeveralStates<::state_list::End, TRANSITION_LIST> {
        using state_list = ::state_list::End;
        using transition_list = ::transition_list::End;
    };

    template<typename DESTS, typename TRANSITION_LIST>
    struct ReverseEpsilonTransitionsToSeveralStates;

    template<size_t DEST, typename TRANSITION_LIST>
    struct ReverseEpsilonTransitionsToOneState {
        using prevs = ::transition_list::FindToT<DEST, TRANSITION_LIST>;
        using epsilon_transitions = OnlyEpsilonTransitionsT<prevs>;
        using non_epsilon_transitions = RemoveEpsilonTransitionsT<prevs>;

        using next_results = ReverseEpsilonTransitionsToSeveralStates<::transition_list::GetFromsT<epsilon_transitions>, TRANSITION_LIST>;

        using state_list = ::state_list::ConcatenateT<
                ::transition_list::GetFromsT<epsilon_transitions>,
                typename next_results::state_list>;

        using transition_list = ::transition_list::ConcatenateT<
                non_epsilon_transitions,
                typename next_results::transition_list>;
    };

    template<typename DESTS, typename TRANSITION_LIST>
    struct ReverseEpsilonTransitionsToSeveralStates {
        using reverse_current = ReverseEpsilonTransitionsToOneState<DESTS::state, TRANSITION_LIST>;

        using next_results = ReverseEpsilonTransitionsToSeveralStates<typename DESTS::next, TRANSITION_LIST>;

        using state_list = ::state_list::ConcatenateT<
                typename reverse_current::state_list,
                typename next_results::state_list>;

        using transition_list = ::transition_list::ConcatenateT<
                typename reverse_current::transition_list,
                typename next_results::transition_list>;
    };

    template<typename TRANSITION_LIST>
    struct ReverseEpsilonTransitionsToSeveralStates<::state_list::End, TRANSITION_LIST> {
        using state_list = ::state_list::End;
        using transition_list = ::transition_list::End;
    };

    template<size_t CURRENT_STATE, size_t STATES_COUNT, typename TRANSITION_LIST>
    struct UpdateTransitions {
        using transition_list = ::transition_list::ConcatenateT<
                ::transition_list::UpdateFromT<CURRENT_STATE, typename FollowEpsilonTransitionsFromOneState<CURRENT_STATE, TRANSITION_LIST>::transition_list>,
                typename UpdateTransitions<CURRENT_STATE + 1, STATES_COUNT, TRANSITION_LIST>::transition_list>;
    };

    template<size_t STATES_COUNT, typename TRANSITION_LIST>
    struct UpdateTransitions<STATES_COUNT, STATES_COUNT, TRANSITION_LIST> {
        using transition_list = ::transition_list::End;
    };

    template<typename STATE_MACHINE>
    using EpsilonRemovalT = StateMachine<STATE_MACHINE::states_count,
                                         ::state_list::Node<STATE_MACHINE::init_state, typename FollowEpsilonTransitionsFromOneState<STATE_MACHINE::init_state, typename STATE_MACHINE::transition_list>::state_list>,
                                         ::state_list::Node<STATE_MACHINE::final_state, typename ReverseEpsilonTransitionsToOneState<STATE_MACHINE::final_state, typename STATE_MACHINE::transition_list>::state_list>,
                                         RemoveEpsilonTransitionsT<typename UpdateTransitions<0, STATE_MACHINE::states_count, typename STATE_MACHINE::transition_list>::transition_list>>;
}// namespace nfa
