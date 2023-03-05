#pragma once

#include "list.hpp"

namespace helpers {
    namespace detail {
        // Wraps the key to prevent issues with keys being convertible to one another
        template<typename Key>
        struct map_key_wrapper {
            using key = Key;
        };
    }// namespace detail

    template<typename Key, typename Value>
    struct map_entry {
        using key = Key;
        using value = Value;
    };

    template<typename MapEntryList>
    struct map {
        using map_entry_list = MapEntryList;
    };

    struct map_not_found {};

    template<typename Map>
    struct map_list {
        using type = typename Map::map_entry_list;
    };

    template<typename Map>
    using map_list_t = typename map_list<Map>::type;

    namespace detail {
        struct map_keys_mapper {
            template<typename MapEntry>
            using type = typename MapEntry::key;
        };
    }// namespace detail

    template<typename Map>
    struct map_keys {
        using type = list_map_t<typename Map::map_entry_list, detail::map_keys_mapper>;
    };

    template<typename Map>
    using map_keys_t = typename map_keys<Map>::type;

    namespace detail {
        struct map_values_mapper {
            template<typename MapEntry>
            using type = typename MapEntry::value;
        };
    }// namespace detail

    template<typename Map>
    struct map_values {
        using type = list_map_t<typename Map::map_entry_list, detail::map_values_mapper>;
    };

    template<typename Map>
    using map_values_t = typename map_values<Map>::type;

    namespace detail {
        template<typename Key>
        struct map_key_matches {
            template<typename Value>
            static constexpr bool value = std::is_same_v<typename Value::key, Key>;
        };
    }// namespace detail

    template<typename Map, typename Key>
    struct map_contains {
        static constexpr bool value = !std::is_same_v<list_find_t<typename Map::map_entry_list, detail::map_key_matches<Key>>, list_not_found>;
    };

    template<typename Map, typename Key>
    constexpr auto map_contains_v = map_contains<Map, Key>::value;

    template<typename Map, typename Key>
    struct map_find {
        using search_result = list_find_t<typename Map::map_entry_list, detail::map_key_matches<Key>>;

        using type = decltype(([] {
            if constexpr (std::is_same_v<search_result, list_not_found>) {
                return map_not_found{};
            } else {
                return typename search_result::value{};
            }
        })());
    };

    template<typename Map, typename Key>
    using map_find_t = typename map_find<Map, Key>::type;

    namespace detail {
        template<typename MapEntryList, typename Key, typename Value, bool replace>
        struct map_add_impl {
            using next = map_add_impl<typename MapEntryList::next, Key, Value, replace>;

            using type = list_node<typename MapEntryList::value, typename next::type>;
            using result_type = typename next::result_type;
        };

        template<typename Next, typename EntryValue, typename Key, typename Value>
        struct map_add_impl<list_node<map_entry<Key, EntryValue>, Next>, Key, Value, true> {
            using type = list_node<map_entry<Key, Value>, Next>;
            using result_type = Value;
        };

        template<typename Next, typename EntryValue, typename Key, typename Value>
        struct map_add_impl<list_node<map_entry<Key, EntryValue>, Next>, Key, Value, false> {
            using type = list_node<map_entry<Key, EntryValue>, Next>;
            using result_type = EntryValue;
        };

        template<typename Key, typename Value, bool replace>
        struct map_add_impl<list_end, Key, Value, replace> {
            using type = list_node<map_entry<Key, Value>, list_end>;
            using result_type = Value;
        };
    }// namespace detail

    // Does not overwrite
    template<typename Map, typename Key, typename Value>
    struct map_insert {
        using helper = detail::map_add_impl<typename Map::map_entry_list, Key, Value, false>;

        using type = map<typename helper::type>;
        using result_type = typename helper::result_type;
    };

    template<typename Map, typename Key, typename Value>
    using map_insert_t = typename map_insert<Map, Key, Value>::type;

    template<typename Map, typename Key, typename Value>
    using map_insert_result_t = typename map_insert<Map, Key, Value>::result_type;

    // Overwrites
    template<typename Map, typename Key, typename Value>
    struct map_set {
        using type = map<typename detail::map_add_impl<typename Map::map_entry_list, Key, Value, true>::type>;
    };

    template<typename Map, typename Key, typename Value>
    using map_set_t = typename map_set<Map, Key, Value>::type;
}// namespace helpers
