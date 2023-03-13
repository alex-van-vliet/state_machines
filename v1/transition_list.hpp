#pragma once

#include "state_list.hpp"
#include "types.hpp"

namespace transition_list {
    template<typename TRANSITION, size_t FROM, size_t TO>
    struct Entry {
        using transition = TRANSITION;
        constexpr static size_t from = FROM;
        constexpr static size_t to = TO;
    };

    struct End {};

    template<typename ENTRY, typename NEXT = End>
    struct Node {
        using entry = ENTRY;
        using next = NEXT;
    };

    template<typename LIST, typename FUNCTOR>
    struct ForEach {
        void operator()() {
            FUNCTOR{}.template operator()<typename LIST::entry::transition, LIST::entry::from, LIST::entry::to>();
            ForEach<typename LIST::next, FUNCTOR>{}();
        };
    };

    template<typename FUNCTOR>
    struct ForEach<End, FUNCTOR> {
        void operator()() {}
    };


    template<typename TRANSITION_LIST_1, typename TRANSITION_LIST_2>
    struct Concatenate {
        using transition_list = Node<typename TRANSITION_LIST_1::entry,
                                     typename Concatenate<typename TRANSITION_LIST_1::next, TRANSITION_LIST_2>::transition_list>;
    };

    template<typename TRANSITION_LIST_2>
    struct Concatenate<End, TRANSITION_LIST_2> {
        using transition_list = TRANSITION_LIST_2;
    };

    template<typename TRANSITION_LIST_1, typename TRANSITION_LIST_2>
    using ConcatenateT = typename Concatenate<TRANSITION_LIST_1, TRANSITION_LIST_2>::transition_list;

    template<typename TRANSITION_LIST>
    struct GetFroms {
        using state_list = state_list::Node<TRANSITION_LIST::entry::from, typename GetFroms<typename TRANSITION_LIST::next>::state_list>;
    };

    template<>
    struct GetFroms<End> {
        using state_list = state_list::End;
    };

    template<typename TRANSITION_LIST>
    using GetFromsT = typename GetFroms<TRANSITION_LIST>::state_list;

    template<typename TRANSITION_LIST>
    struct GetTos {
        using state_list = state_list::Node<TRANSITION_LIST::entry::to, typename GetTos<typename TRANSITION_LIST::next>::state_list>;
    };

    template<>
    struct GetTos<End> {
        using state_list = state_list::End;
    };

    template<typename TRANSITION_LIST>
    using GetTosT = typename GetTos<TRANSITION_LIST>::state_list;

    template<size_t FROM, typename TRANSITION_LIST>
    struct FindFrom {
    };

    template<size_t FROM, size_t TO, typename TRANSITION, typename NEXT>
    struct FindFrom<FROM, Node<Entry<TRANSITION, FROM, TO>, NEXT>> {
        using transition_list = Node<Entry<TRANSITION, FROM, TO>, typename FindFrom<FROM, NEXT>::transition_list>;
    };

    template<size_t FROM, typename ENTRY, typename NEXT>
    struct FindFrom<FROM, Node<ENTRY, NEXT>> {
        using transition_list = typename FindFrom<FROM, NEXT>::transition_list;
    };

    template<size_t FROM>
    struct FindFrom<FROM, End> {
        using transition_list = End;
    };

    template<size_t FROM, typename TRANSITION_LIST>
    using FindFromT = typename FindFrom<FROM, TRANSITION_LIST>::transition_list;

    template<typename FROMS, typename TRANSITION_LIST>
    struct FindFroms {
        using transition_list = ConcatenateT<
                FindFromT<FROMS::state, TRANSITION_LIST>,
                typename FindFroms<typename FROMS::next, TRANSITION_LIST>::transition_list>;
    };

    template<typename TRANSITION_LIST>
    struct FindFroms<state_list::End, TRANSITION_LIST> {
        using transition_list = End;
    };

    template<typename FROMS, typename TRANSITION_LIST>
    using FindFromsT = typename FindFroms<FROMS, TRANSITION_LIST>::transition_list;

    template<size_t TO, typename TRANSITION_LIST>
    struct FindTo {};

    template<size_t TO, size_t FROM, typename TRANSITION, typename NEXT>
    struct FindTo<TO, Node<Entry<TRANSITION, FROM, TO>, NEXT>> {
        using transition_list = Node<Entry<TRANSITION, FROM, TO>, typename FindTo<TO, NEXT>::transition_list>;
    };

    template<size_t TO, typename ENTRY, typename NEXT>
    struct FindTo<TO, Node<ENTRY, NEXT>> {
        using transition_list = typename FindTo<TO, NEXT>::transition_list;
    };

    template<size_t TO>
    struct FindTo<TO, End> {
        using transition_list = End;
    };

    template<size_t TO, typename TRANSITION_LIST>
    using FindToT = typename FindTo<TO, TRANSITION_LIST>::transition_list;

    template<size_t NEW_FROM, typename TRANSITION_LIST>
    struct UpdateFrom {
        using transition_list = Node<
                Entry<typename TRANSITION_LIST::entry::transition, NEW_FROM, TRANSITION_LIST::entry::to>,
                typename UpdateFrom<NEW_FROM, typename TRANSITION_LIST::next>::transition_list>;
    };

    template<size_t NEW_FROM>
    struct UpdateFrom<NEW_FROM, End> {
        using transition_list = End;
    };

    template<size_t NEW_FROM, typename TRANSITION_LIST>
    using UpdateFromT = typename UpdateFrom<NEW_FROM, TRANSITION_LIST>::transition_list;


    template<size_t TO>
    struct TransitionFound {
        constexpr static size_t to = TO;
    };
    struct TransitionNotFound {};
    template<size_t FROM, typename TRANSITION, typename TRANSITION_LIST>
    struct FindToWithTransition {
        using result = typename FindToWithTransition<FROM, TRANSITION, typename TRANSITION_LIST::next>::result;
    };
    template<size_t FROM, typename TRANSITION, size_t TO, typename NEXT>
    struct FindToWithTransition<FROM, TRANSITION, Node<Entry<TRANSITION, FROM, TO>, NEXT>> {
        using result = TransitionFound<TO>;
    };

    template<size_t FROM, typename TRANSITION>
    struct FindToWithTransition<FROM, TRANSITION, End> {
        using result = TransitionNotFound;
    };

    template<size_t FROM, typename TRANSITION, typename TRANSITION_LIST>
    using FindToWithTransitionT = typename FindToWithTransition<FROM, TRANSITION, TRANSITION_LIST>::result;
    template<size_t FROM, typename TRANSITION, typename TRANSITION_LIST>
    constexpr size_t FindToWithTransitionV = FindToWithTransition<FROM, TRANSITION, TRANSITION_LIST>::result::to;
}// namespace transition_list
