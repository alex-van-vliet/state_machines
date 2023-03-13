#pragma once

#include "types.hpp"

namespace state_list_list {
    struct End {};

    template<typename STATE_LIST, typename NEXT = End>
    struct Node {
        using state_list = STATE_LIST;
        using next = NEXT;
    };

    template<typename STATE_LIST_LIST>
    struct Length {
        constexpr static size_t length = Length<typename STATE_LIST_LIST::next>::length + 1;
    };

    template<>
    struct Length<End> {
        constexpr static size_t length = 0;
    };

    template<typename STATE_LIST_LIST>
    constexpr size_t LengthV = Length<STATE_LIST_LIST>::length;

    template<typename STATE_LIST_LIST, typename STATES, size_t INDEX = 0>
    struct CreateIfNotExists {
        using state_list_list = Node<typename STATE_LIST_LIST::state_list, typename CreateIfNotExists<typename STATE_LIST_LIST::next, STATES, INDEX + 1>::state_list_list>;
    };

    template<typename NEXT, typename STATES, size_t INDEX>
    struct CreateIfNotExists<Node<STATES, NEXT>, STATES, INDEX> {
        using state_list_list = Node<STATES, NEXT>;
    };

    template<typename STATES, size_t INDEX>
    struct CreateIfNotExists<End, STATES, INDEX> {
        using state_list_list = Node<STATES>;
    };

    template<typename STATE_LIST_LIST, typename STATES>
    using CreateIfNotExistsT = typename CreateIfNotExists<STATE_LIST_LIST, STATES>::state_list_list;

    template<typename STATE_LIST_LIST, typename STATES, size_t INDEX = 0>
    struct FindIndex {
        constexpr static size_t index = FindIndex<typename STATE_LIST_LIST::next, STATES, INDEX + 1>::index;
    };

    template<typename NEXT, typename STATES, size_t INDEX>
    struct FindIndex<Node<STATES, NEXT>, STATES, INDEX> {
        constexpr static size_t index = INDEX;
    };

    template<typename STATE_LIST_LIST, typename STATES>
    constexpr size_t FindIndexV = FindIndex<STATE_LIST_LIST, STATES>::index;

    template<typename STATE_LIST_LIST, size_t INDEX>
    struct At {
        using state_list = typename At<typename STATE_LIST_LIST::next, INDEX - 1>::state_list;
    };

    template<typename STATE_LIST_LIST>
    struct At<STATE_LIST_LIST, 0> {
        using state_list = typename STATE_LIST_LIST::state_list;
    };

    template<typename STATE_LIST_LIST, size_t INDEX>
    using AtT = typename At<STATE_LIST_LIST, INDEX>::state_list;
}// namespace state_list_list
