#pragma once

#include "state_list.hpp"
#include "types.hpp"

namespace char_state_list_map {
    struct End {};

    template<char C, typename STATE_LIST, typename NEXT = End>
    struct Node {
        constexpr static char c = C;
        using state_list = STATE_LIST;
        using next = NEXT;
    };

    template<typename CHAR_STATE_LIST_MAP, char C, size_t STATE>
    struct AppendState {
        using char_state_list_map = Node<CHAR_STATE_LIST_MAP::c, typename CHAR_STATE_LIST_MAP::state_list, typename AppendState<typename CHAR_STATE_LIST_MAP::next, C, STATE>::char_state_list_map>;
    };

    template<typename STATE_LIST, typename NEXT, char C, size_t STATE>
    struct AppendState<Node<C, STATE_LIST, NEXT>, C, STATE> {
        using char_state_list_map = Node<C, ::state_list::Node<STATE, STATE_LIST>, NEXT>;
    };

    template<char C, size_t STATE>
    struct AppendState<End, C, STATE> {
        using char_state_list_map = Node<C, ::state_list::Node<STATE>>;
    };

    template<typename CHAR_STATE_LIST_MAP, char C, size_t STATE>
    using AppendStateT = typename AppendState<CHAR_STATE_LIST_MAP, C, STATE>::char_state_list_map;
}// namespace char_state_list_map
