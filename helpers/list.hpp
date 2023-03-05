#pragma once

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <limits>
#include <type_traits>

namespace helpers {
    template<typename Value, typename Next>
    struct list_node {
        using value = Value;
        using next = Next;

        static constexpr std::size_t length = 1 + Next::length;
    };

    struct list_end {
        static constexpr std::size_t length = 0;
    };

    struct list_not_found {};

    template<typename... Values>
    struct list_construct {
    };

    template<typename Value, typename... Values>
    struct list_construct<Value, Values...> {
        using type = list_node<Value, typename list_construct<Values...>::type>;
    };

    template<>
    struct list_construct<> {
        using type = list_end;
    };

    template<typename... Values>
    using list_construct_t = typename list_construct<Values...>::type;

    template<typename List>
    struct list_length {
        static constexpr auto value = List::length;
    };

    template<typename List>
    constexpr auto list_length_v = list_length<List>::value;

    template<typename... Lists>
    struct list_concat {};

    template<typename List, typename... Lists>
    struct list_concat<List, Lists...> {
        using type = decltype(([]{
            if constexpr (std::is_same_v<List, list_end>) {
                return typename list_concat<Lists...>::type{};
            } else {
                return list_node<typename List::value, typename list_concat<typename List::next, Lists...>::type>{};
            }
        })());
    };

    template<typename List>
    struct list_concat<List> {
        using type = List;
    };

    template<>
    struct list_concat<> {
        using type = list_end;
    };

    template<typename... Lists>
    using list_concat_t = typename list_concat<Lists...>::type;

    template<typename List, typename... PushValues>
    struct list_push_back {
        using type = list_concat_t<List, list_construct_t<PushValues...>>;
    };

    template<typename List, typename... PushValues>
    using list_push_back_t = typename list_push_back<List, PushValues...>::type;

    template<typename List, typename... PushValues>
    struct list_push_front {
        using type = list_concat_t<list_construct_t<PushValues...>, List>;
    };

    template<typename List, typename... PushValues>
    using list_push_front_t = typename list_push_front<List, PushValues...>::type;

    // Functor must implement Functor{}(Value)
    template<typename List, typename Functor>
    struct list_apply {
        void operator()() const {
            Functor{}(typename List::value{});
            list_apply<typename List::next, Functor>{}();
        }
    };

    template<typename Functor>
    struct list_apply<list_end, Functor> {
        void operator()() const {}
    };

    // Mapping must implement Mapping::type<Value>
    template<typename List, typename Mapping>
    struct list_map {
        using type = list_node<typename Mapping::template type<typename List::value>,
                               typename list_map<typename List::next, Mapping>::type>;
    };

    template<typename Mapping>
    struct list_map<list_end, Mapping> {
        using type = list_end;
    };

    template<typename List, typename Mapping>
    using list_map_t = typename list_map<List, Mapping>::type;

    // Predicate must implement Predicate::value<Value>
    template<typename List, typename Predicate>
    struct list_filter {
        using type = decltype(([] {
            if constexpr (Predicate::template value<typename List::value>) {
                return list_node<typename List::value, typename list_filter<typename List::next, Predicate>::type>{};
            } else {
                return typename list_filter<typename List::next, Predicate>::type{};
            }
        })());
    };

    template<typename Predicate>
    struct list_filter<list_end, Predicate> {
        using type = list_end;
    };

    template<typename List, typename Predicate>
    using list_filter_t = typename list_filter<List, Predicate>::type;

    template<typename Predicate>
    struct predicate_not {
        template<typename Value>
        static constexpr bool value = !Predicate::template value<Value>;
    };

    template<typename... Predicates>
    struct predicate_or {
        template<typename Value>
        static constexpr bool value = (Predicates::template value<Value> || ...);
    };

    template<typename... Predicates>
    struct predicate_and {
        template<typename Value>
        static constexpr bool value = (Predicates::template value<Value> && ...);
    };

    // Predicate must implement Predicate::value<Value>
    template<typename List, typename Predicate>
    struct list_find {
        using type = decltype(([] {
            if constexpr (Predicate::template value<typename List::value>) {
                return typename List::value{};
            } else {
                return typename list_find<typename List::next, Predicate>::type{};
            }
        })());
    };

    template<typename Predicate>
    struct list_find<list_end, Predicate> {
        using type = list_not_found;
    };

    template<typename List, typename Predicate>
    using list_find_t = typename list_find<List, Predicate>::type;

    template<typename List, typename Search>
    struct list_contains {
        static constexpr bool value = list_contains<typename List::next, Search>::value;
    };

    template<typename List, typename Search>
    struct list_contains<list_node<Search, List>, Search> {
        static constexpr bool value = true;
    };

    template<typename Search>
    struct list_contains<list_end, Search> {
        static constexpr bool value = false;
    };

    template<typename List, typename Search>
    constexpr auto list_contains_v = list_contains<List, Search>::value;

    template<typename List, typename Search>
    struct list_remove {
        using next = list_remove<typename List::next, Search>;

        using type = list_node<typename List::value, typename next::type>;
        using result_type = typename next::result_type;
    };

    template<typename List, typename Search>
    struct list_remove<list_node<Search, List>, Search> {
        using type = List;
        using result_type = Search;
    };

    template<typename Search>
    struct list_remove<list_end, Search> {
        using type = list_end;
        using result_type = list_not_found;
    };

    template<typename List, typename Search>
    using list_remove_t = typename list_remove<List, Search>::type;

    template<typename List, typename Search>
    using list_remove_result_t = typename list_remove<List, Search>::result_type;

    namespace detail {
        template<typename Value, auto Key>
        struct list_sort_vk_pair {
            using value = Value;
            static constexpr auto key = Key;
        };

        template<typename Key>
        struct list_sort_add_key {
            template<typename Value>
            using type = list_sort_vk_pair<Value, Key::template value<Value>>;
        };

        struct list_sort_remove_key {
            template<typename Pair>
            using type = typename Pair::value;
        };

        template<typename ListWithKeys, auto RunningMinValue>
        struct list_sort_find_mins {
            using pair = list_sort_vk_pair<typename ListWithKeys::value::value, ListWithKeys::value::key>;

            using rec = list_sort_find_mins<typename ListWithKeys::next, std::min(ListWithKeys::value::key, RunningMinValue)>;

            static constexpr auto value = rec::value;
            using type = std::conditional_t<value == ListWithKeys::value::key, list_node<pair, typename rec::type>, typename rec::type>;
            using residual_type = std::conditional_t<value == ListWithKeys::value::key, typename rec::residual_type, list_node<pair, typename rec::residual_type>>;
        };

        template<auto RunningMinValue>
        struct list_sort_find_mins<list_end, RunningMinValue> {
            static constexpr auto value = RunningMinValue;
            using type = list_end;
            using residual_type = list_end;
        };

        template<typename ListWithKeys>
        struct list_sort_min_init {
            static constexpr auto value = ([] {
                if constexpr (std::numeric_limits<decltype(ListWithKeys::value::key)>::has_infinity) {
                    return std::numeric_limits<decltype(ListWithKeys::value::key)>::infinity();
                }
                return std::numeric_limits<decltype(ListWithKeys::value::key)>::max();
            })();
        };

        template<typename ListWithKeys>
        struct list_sort_impl {
            using mins = list_sort_find_mins<ListWithKeys, list_sort_min_init<ListWithKeys>::value>;

            using type = list_concat_t<typename mins::type, typename list_sort_impl<typename mins::residual_type>::type>;
        };

        template<>
        struct list_sort_impl<list_end> {
            using type = list_end;
        };
    }// namespace detail

    // Key must implement Key::value<Value> as constexpr of the same type (U.B. otherwise)
    template<typename List, typename Key>
    struct list_sort {
        using list_with_keys = list_map_t<List, detail::list_sort_add_key<Key>>;

        using sorted_list_with_keys = typename detail::list_sort_impl<list_with_keys>::type;

        using type = list_map_t<sorted_list_with_keys, detail::list_sort_remove_key>;
    };

    template<typename List, typename Key>
    using list_sort_t = typename list_sort<List, Key>::type;

    // As in the standard lib, unique removes *consecutive* duplicate elements
    // EqualityComparator must implement EqualityComparator::value<ValueA, ValueB> as constexpr bool
    template<typename List, typename EqualityComparator>
    struct list_unique {
    };

    template<typename ValueA, typename ValueB, typename Next, typename EqualityComparator>
    struct list_unique<list_node<ValueA, list_node<ValueB, Next>>, EqualityComparator> {
        using type = decltype(([]() {
            if constexpr (EqualityComparator::template value<ValueA, ValueB>) {
                return typename list_unique<list_node<ValueB, Next>, EqualityComparator>::type{};
            } else {
                return list_node<ValueA, typename list_unique<list_node<ValueB, Next>, EqualityComparator>::type>{};
            }
        })());
    };

    template<typename Value, typename EqualityComparator>
    struct list_unique<list_node<Value, list_end>, EqualityComparator> {
        using type = list_node<Value, list_end>;
    };

    template<typename EqualityComparator>
    struct list_unique<list_end, EqualityComparator> {
        using type = list_end;
    };

    template<typename List, typename EqualityComparator>
    using list_unique_t = typename list_unique<List, EqualityComparator>::type;
}// namespace helpers
