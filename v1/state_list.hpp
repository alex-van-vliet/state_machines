#pragma once

#include "types.hpp"
#include <algorithm>
#include <limits>

namespace state_list {
    struct End {};

    template<size_t STATE, typename NEXT = End>
    struct Node {
        constexpr static size_t state = STATE;
        using next = NEXT;
    };

    template<typename LIST, typename FUNCTOR>
    struct ForEach {
        void operator()() {
            FUNCTOR{}.template operator()<LIST::state>();
            ForEach<typename LIST::next, FUNCTOR>{}();
        };
    };

    template<typename FUNCTOR>
    struct ForEach<End, FUNCTOR> {
        void operator()() {}
    };

    template<typename HAYSTACK_STATE_LIST, size_t NEEDLE_STATE>
    struct Contains {
        constexpr static bool contains = Contains<typename HAYSTACK_STATE_LIST::next, NEEDLE_STATE>::contains;
    };

    template<typename NEXT, size_t NEEDLE_STATE>
    struct Contains<Node<NEEDLE_STATE, NEXT>, NEEDLE_STATE> {
        constexpr static bool contains = true;
    };

    template<size_t NEEDLE_STATE>
    struct Contains<End, NEEDLE_STATE> {
        constexpr static bool contains = false;
    };

    template<typename HAYSTACK_STATE_LIST, size_t NEEDLE_STATE>
    constexpr bool ContainsV = Contains<HAYSTACK_STATE_LIST, NEEDLE_STATE>::contains;

    template<typename STATE_LIST_1, typename STATE_LIST_2>
    struct Concatenate {
        using state_list = Node<STATE_LIST_1::state,
                                typename Concatenate<typename STATE_LIST_1::next, STATE_LIST_2>::state_list>;
    };

    template<typename STATE_LIST_2>
    struct Concatenate<End, STATE_LIST_2> {
        using state_list = STATE_LIST_2;
    };

    template<typename STATE_LIST_1, typename STATE_LIST_2>
    using ConcatenateT = typename Concatenate<STATE_LIST_1, STATE_LIST_2>::state_list;

    constexpr size_t ThresholdedMinFunction(size_t lower_bound, size_t running_min, size_t new_value) {
        if (new_value < lower_bound)
            return running_min;
        return std::min(new_value, running_min);
    }

    template<typename STATE_LIST, size_t LOWER_BOUND, size_t RUNNING_MIN = std::numeric_limits<size_t>::max()>
    struct ThresholdedMin {
        constexpr static size_t current_result = ThresholdedMinFunction(LOWER_BOUND, RUNNING_MIN, STATE_LIST::state);

        constexpr static size_t min = ThresholdedMin<typename STATE_LIST::next, LOWER_BOUND, current_result>::min;
    };

    template<size_t LOWER_BOUND, size_t RUNNING_MIN>
    struct ThresholdedMin<::state_list::End, LOWER_BOUND, RUNNING_MIN> {
        constexpr static size_t min = RUNNING_MIN;
    };

    template<typename STATE_LIST, size_t LOWER_BOUND, size_t RUNNING_MIN = std::numeric_limits<size_t>::max()>
    constexpr size_t ThresholdedMinV = ThresholdedMin<STATE_LIST, LOWER_BOUND, RUNNING_MIN>::min;

    template<typename STATE_LIST, size_t LOWER_BOUND = 0>
    struct UniqueSort;

    template<typename STATE_LIST, size_t MIN>
    struct UniqueSort_Helper {
        using next_result = UniqueSort<STATE_LIST, MIN + 1>;

        using state_list = ::state_list::Node<MIN, typename next_result::state_list>;
    };

    template<typename STATE_LIST>
    struct UniqueSort_Helper<STATE_LIST, std::numeric_limits<size_t>::max()> {
        using state_list = ::state_list::End;
    };

    template<typename STATE_LIST, size_t LOWER_BOUND>
    struct UniqueSort {
        constexpr static size_t min = ThresholdedMinV<STATE_LIST, LOWER_BOUND>;

        using helper = UniqueSort_Helper<STATE_LIST, min>;

        using state_list = typename helper::state_list;
    };

    template<typename STATE_LIST>
    using UniqueSortT = typename UniqueSort<STATE_LIST>::state_list;
}// namespace state_list
