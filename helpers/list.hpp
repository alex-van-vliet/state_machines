#pragma once

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <limits>
#include <type_traits>

namespace helpers {
    template<typename... Values>
    struct list {
        static constexpr std::size_t length = sizeof...(Values);
    };

    struct list_not_found {};

    template<typename List>
    struct list_length {
        static constexpr auto value = List::length;
    };

    template<typename List>
    constexpr auto list_length_v = list_length<List>::value;

    template<typename... Lists>
    struct list_concat {};

    template<typename... ValuesA, typename... ValuesB, typename... Lists>
    struct list_concat<list<ValuesA...>, list<ValuesB...>, Lists...> {
        using type = typename list_concat<list<ValuesA..., ValuesB...>, Lists...>::type;
    };

    template<typename List>
    struct list_concat<List> {
        using type = List;
    };

    template<>
    struct list_concat<list<>> {
        using type = list<>;
    };

    template<typename... Lists>
    using list_concat_t = typename list_concat<Lists...>::type;

    template<typename List, typename... PushValues>
    struct list_push_back {
        using type = list_concat_t<List, list<PushValues...>>;
    };

    template<typename List, typename... PushValues>
    using list_push_back_t = typename list_push_back<List, PushValues...>::type;

    template<typename List, typename... PushValues>
    struct list_push_front {
        using type = list_concat_t<list<PushValues...>, List>;
    };

    template<typename List, typename... PushValues>
    using list_push_front_t = typename list_push_front<List, PushValues...>::type;

    // Functor must implement Functor{}(Value)
    template<typename List, typename Functor>
    struct list_apply {};

    template<typename... Values, typename Functor>
    struct list_apply<list<Values...>, Functor> {
        void operator()() const {
            ((void) Functor{}(Values{}), ...);
        }
    };

    // Mapping must implement Mapping::type<Value>
    template<typename List, typename Mapping>
    struct list_map {};

    template<typename... Values, typename Mapping>
    struct list_map<list<Values...>, Mapping> {
        using type = list<typename Mapping::template type<Values>...>;
    };

    template<typename List, typename Mapping>
    using list_map_t = typename list_map<List, Mapping>::type;

    // Predicate must implement Predicate::value<Value>
    template<typename List, typename Predicate>
    struct list_filter {};

    template<typename Value, typename... Values, typename Predicate>
    struct list_filter<list<Value, Values...>, Predicate> {
        using type = std::conditional_t<
                Predicate::template value<Value>,
                list_push_front_t<typename list_filter<list<Values...>, Predicate>::type, Value>,
                typename list_filter<list<Values...>, Predicate>::type>;
    };

    template<typename Predicate>
    struct list_filter<list<>, Predicate> {
        using type = list<>;
    };

    template<typename List, typename Predicate>
    using list_filter_t = typename list_filter<List, Predicate>::type;

    template<typename Predicate>
    struct predicate_not
    {
        template<typename Value>
        static constexpr bool value = !Predicate::template value<Value>;
    };

    template<typename... Predicates>
    struct predicate_or
    {
        template<typename Value>
        static constexpr bool value = (Predicates::template value<Value> || ...);
    };

    template<typename... Predicates>
    struct predicate_and
    {
        template<typename Value>
        static constexpr bool value = (Predicates::template value<Value> &&...);
    };

    // Predicate must implement Predicate::value<Value>
    template<typename List, typename Predicate>
    struct list_find {};

    template<typename Value, typename... Values, typename Predicate>
    struct list_find<list<Value, Values...>, Predicate> {
        using type = decltype(([] {
            if constexpr (Predicate::template value<Value>) {
                return Value{};
            } else {
                return typename list_filter<list<Values...>, Predicate>::type{};
            }
        })());
    };

    template<typename Predicate>
    struct list_find<list<>, Predicate> {
        using type = list_not_found;
    };

    template<typename List, typename Predicate>
    using list_find_t = typename list_find<List, Predicate>::type;

    template<typename List, typename Search>
    struct list_contains {};

    template<typename... Values, typename Search>
    struct list_contains<list<Values...>, Search> {
        static constexpr bool value = (std::is_same_v<Values, Search> || ...);
    };

    template<typename List, typename Search>
    constexpr auto list_contains_v = list_contains<List, Search>::value;

    template<typename List, typename Search>
    struct list_remove {};

    template<typename... Values, typename Search>
    struct list_remove<list<Search, Values...>, Search> {
        using type = list<Values...>;
        using result_type = Search;
    };

    template<typename Value, typename... Values, typename Search>
    struct list_remove<list<Value, Values...>, Search> {
        using next = list_remove<list<Values...>, Search>;

        using type = list_push_front_t<typename next::type, Value>;
        using result_type = typename next::result_type;
    };

    template<typename Search>
    struct list_remove<list<>, Search> {
        using type = list<>;
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
        struct list_sort_find_mins {};

        template<typename Value, auto Key, typename... ValueKeyPairs, auto RunningMinValue>
        struct list_sort_find_mins<list<list_sort_vk_pair<Value, Key>, ValueKeyPairs...>, RunningMinValue> {
            using pair = list_sort_vk_pair<Value, Key>;

            using rec = list_sort_find_mins<list<ValueKeyPairs...>, std::min(Key, RunningMinValue)>;

            static constexpr auto value = rec::value;
            using type = std::conditional_t<value == Key, list_push_front_t<typename rec::type, pair>, typename rec::type>;
            using residual_type = std::conditional_t<value == Key, typename rec::residual_type, list_push_front_t<typename rec::residual_type, pair>>;
        };

        template<auto RunningMinValue>
        struct list_sort_find_mins<list<>, RunningMinValue> {
            static constexpr auto value = RunningMinValue;
            using type = list<>;
            using residual_type = list<>;
        };

        template<typename ListWithKeys>
        struct list_sort_min_init {};

        template<typename Value, auto Key, typename... ValueKeyPairs>
        struct list_sort_min_init<list<list_sort_vk_pair<Value, Key>, ValueKeyPairs...>> {
            static constexpr auto value = ([] {
                if constexpr (std::numeric_limits<decltype(Key)>::has_infinity) {
                    return std::numeric_limits<decltype(Key)>::infinity();
                }
                return std::numeric_limits<decltype(Key)>::max();
            })();
        };

        template<typename ListWithKeys>
        struct list_sort_impl {
            using mins = list_sort_find_mins<ListWithKeys, list_sort_min_init<ListWithKeys>::value>;

            using type = list_concat_t<typename mins::type, typename list_sort_impl<typename mins::residual_type>::type>;
        };

        template<>
        struct list_sort_impl<list<>> {
            using type = list<>;
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

    template<typename ValueA, typename ValueB, typename... Values, typename EqualityComparator>
    struct list_unique<list<ValueA, ValueB, Values...>, EqualityComparator> {
        using type = decltype(([](){
            if constexpr (EqualityComparator::template value<ValueA, ValueB>) {
                return typename list_unique<list<ValueA, Values...>, EqualityComparator>::type{};
            } else {
                return list_push_front_t<typename list_unique<list<ValueB, Values...>, EqualityComparator>::type, ValueA>{};
            }
        })());
    };

    template<typename Value, typename EqualityComparator>
    struct list_unique<list<Value>, EqualityComparator> {
        using type = list<Value>;
    };

    template<typename EqualityComparator>
    struct list_unique<list<>, EqualityComparator> {
        using type = list<>;
    };

    template<typename List, typename EqualityComparator>
    using list_unique_t = typename list_unique<List, EqualityComparator>::type;
}// namespace helpers
