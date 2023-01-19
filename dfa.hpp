#pragma once

#include "char_state_list_map.hpp"
#include "state_list.hpp"
#include "state_list_list.hpp"
#include "states.hpp"
#include "transition_list.hpp"
#include "transitions.hpp"
#include "types.hpp"
#include <iostream>
#include <limits>

namespace dfa {
    template<size_t STATES_COUNT, size_t INIT_STATE, typename FINAL_STATE_LIST, typename TRANSITION_LIST>
    struct StateMachine {
        constexpr static size_t states_count = STATES_COUNT;
        constexpr static size_t init_state = INIT_STATE;
        using final_state_list = FINAL_STATE_LIST;
        using transition_list = TRANSITION_LIST;

        static void draw() {
            std::cout << "digraph {\n";
            std::cout << "    invisible [label=\"\", shape=none];\n";
            for (size_t i = 0; i < states_count; ++i) {
                std::cout << "    " << i << ";\n";
            }
            ::state_list::ForEach<final_state_list, ::states::DrawFinal>{}();
            ::states::DrawInit{}.template operator()<INIT_STATE>();
            ::transition_list::ForEach<transition_list, ::transitions::Draw>{}();
            std::cout << "}" << std::endl;
        }
    };

    template<typename TRANSITION_LIST, typename CHAR_STATE_LIST_MAP = ::char_state_list_map::End>
    struct GroupByCharacter {
        using result = char_state_list_map::AppendStateT<CHAR_STATE_LIST_MAP, TRANSITION_LIST::entry::transition::c, TRANSITION_LIST::entry::to>;

        using next_result = GroupByCharacter<typename TRANSITION_LIST::next, result>;

        using char_state_list_map = typename next_result::char_state_list_map;
    };

    template<typename CHAR_STATE_LIST_MAP>
    struct GroupByCharacter<::transition_list::End, CHAR_STATE_LIST_MAP> {
        using char_state_list_map = CHAR_STATE_LIST_MAP;
    };

    template<typename TRANSITION_LIST>
    using GroupByCharacterT = typename GroupByCharacter<TRANSITION_LIST>::char_state_list_map;

    template<typename CHAR_STATE_LIST_MAP>
    struct Clean {
        using char_state_list_map = ::char_state_list_map::Node<
                CHAR_STATE_LIST_MAP::c,
                ::state_list::UniqueSortT<typename CHAR_STATE_LIST_MAP::state_list>,
                typename Clean<typename CHAR_STATE_LIST_MAP::next>::char_state_list_map>;
    };

    template<>
    struct Clean<::char_state_list_map::End> {
        using char_state_list_map = ::char_state_list_map::End;
    };

    template<typename CHAR_STATE_LIST_MAP>
    using CleanT = typename Clean<CHAR_STATE_LIST_MAP>::char_state_list_map;

    template<typename CHAR_STATE_LIST_MAP, typename STATE_LIST_LIST>
    struct AddGroupsToStateListList {
        using result = ::state_list_list::CreateIfNotExistsT<STATE_LIST_LIST, typename CHAR_STATE_LIST_MAP::state_list>;

        using state_list_list = typename AddGroupsToStateListList<typename CHAR_STATE_LIST_MAP::next, result>::state_list_list;
    };

    template<typename STATE_LIST_LIST>
    struct AddGroupsToStateListList<::char_state_list_map::End, STATE_LIST_LIST> {
        using state_list_list = STATE_LIST_LIST;
    };

    template<typename CHAR_STATE_LIST_MAP, typename STATE_LIST_LIST>
    using AddGroupsToStateListListV = typename AddGroupsToStateListList<CHAR_STATE_LIST_MAP, STATE_LIST_LIST>::state_list_list;

    template<typename CHAR_STATE_LIST_MAP, typename STATE_LIST_LIST, size_t FROM>
    struct GroupsToTransitions {
        using transition_list = ::transition_list::Node<
                ::transition_list::Entry<::transitions::Character<CHAR_STATE_LIST_MAP::c>, FROM, ::state_list_list::FindIndexV<STATE_LIST_LIST, typename CHAR_STATE_LIST_MAP::state_list>>,
                typename GroupsToTransitions<typename CHAR_STATE_LIST_MAP::next, STATE_LIST_LIST, FROM>::transition_list>;
    };

    template<typename STATE_LIST_LIST, size_t FROM>
    struct GroupsToTransitions<::char_state_list_map::End, STATE_LIST_LIST, FROM> {
        using transition_list = ::transition_list::End;
    };

    template<typename GROUPS, typename STATE_LIST_LIST, size_t FROM>
    using GroupsToTransitionsT = typename GroupsToTransitions<GROUPS, STATE_LIST_LIST, FROM>::transition_list;

    template<typename TRANSITION_LIST, typename STATE_LIST_LIST, size_t INDEX, size_t SIZE>
    struct _DFAConversionStateIterator {
        using current_state = ::state_list_list::AtT<STATE_LIST_LIST, INDEX>;
        using nexts = ::transition_list::FindFromsT<current_state, TRANSITION_LIST>;
        using groups = GroupByCharacterT<nexts>;
        using cleaned_groups = CleanT<groups>;
        using new_state_list_list = AddGroupsToStateListListV<cleaned_groups, STATE_LIST_LIST>;
        using transition_list_to_add = GroupsToTransitionsT<cleaned_groups, new_state_list_list, INDEX>;

        static constexpr size_t new_state_list_list_length = ::state_list_list::LengthV<new_state_list_list>;
        using next_result = _DFAConversionStateIterator<TRANSITION_LIST, new_state_list_list, INDEX + 1, new_state_list_list_length>;

        using transition_list = transition_list::ConcatenateT<transition_list_to_add, typename next_result::transition_list>;
        using state_list_list = typename next_result::state_list_list;
    };
    template<typename TRANSITION_LIST, typename STATE_LIST_LIST, size_t SIZE>
    struct _DFAConversionStateIterator<TRANSITION_LIST, STATE_LIST_LIST, SIZE, SIZE> {
        using transition_list = ::transition_list::End;
        using state_list_list = STATE_LIST_LIST;
    };

    template<typename HAYSTACK_STATE_LIST, typename NEEDLE_STATE_LIST>
    struct StateListContainsAny {
        constexpr static bool contains = ::state_list::ContainsV<HAYSTACK_STATE_LIST, NEEDLE_STATE_LIST::state> or
                                         StateListContainsAny<HAYSTACK_STATE_LIST, typename NEEDLE_STATE_LIST::next>::contains;
    };

    template<typename HAYSTACK_STATE_LIST>
    struct StateListContainsAny<HAYSTACK_STATE_LIST, ::state_list::End> {
        constexpr static bool contains = false;
    };

    template<typename HAYSTACK_STATE_LIST, typename NEEDLE_STATE_LIST>
    constexpr bool StateListContainsAnyV = StateListContainsAny<HAYSTACK_STATE_LIST, NEEDLE_STATE_LIST>::contains;

    template<typename ORIGINAL_FINAL_STATE_LIST, typename STATE_LIST_LIST, size_t INDEX = 0>
    struct FindFinalStates {
        using current_state_list = typename STATE_LIST_LIST::state_list;

        using next_result = FindFinalStates<ORIGINAL_FINAL_STATE_LIST, typename STATE_LIST_LIST::next, INDEX + 1>;
        using state_list = std::conditional_t<StateListContainsAnyV<ORIGINAL_FINAL_STATE_LIST, current_state_list>,
                                              ::state_list::Node<INDEX, typename next_result::state_list>,
                                              typename next_result::state_list>;
    };

    template<typename ORIGINAL_FINAL_STATE_LIST, size_t INDEX>
    struct FindFinalStates<ORIGINAL_FINAL_STATE_LIST, ::state_list_list::End, INDEX> {
        using state_list = ::state_list::End;
    };

    template<typename ORIGINAL_FINAL_STATE_LIST, typename STATE_LIST_LIST>
    using FindFinalStatesT = typename FindFinalStates<ORIGINAL_FINAL_STATE_LIST, STATE_LIST_LIST>::state_list;

    template<typename STATE_MACHINE>
    struct DFAConversion {
        using result = _DFAConversionStateIterator<typename STATE_MACHINE::transition_list, ::state_list_list::Node<::state_list::UniqueSortT<typename STATE_MACHINE::init_state_list>>, 0, 1>;

        using state_machine = StateMachine<
                ::state_list_list::LengthV<typename result::state_list_list>,
                0, FindFinalStatesT<typename STATE_MACHINE::final_state_list, typename result::state_list_list>,
                typename result::transition_list>;
    };

    template<typename STATE_MACHINE>
    using DFAConversionT = typename DFAConversion<STATE_MACHINE>::state_machine;
}// namespace dfa
