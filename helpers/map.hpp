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

        // Not implemented as only used with decltype
        Value lookup(detail::map_key_wrapper<Key>) const;
    };

    template<typename MapEntryList>
    struct map {
    };

    template<typename... MapEntries>
    struct map<list<MapEntries...>> : MapEntries... {
        using MapEntries::lookup...;
    };

    struct map_not_found {};

    template<typename Map>
    struct map_list {};

    template<typename... MapEntries>
    struct map_list<map<list<MapEntries...>>> {
        using type = list<MapEntries...>;
    };

    template<typename Map>
    using map_list_t = typename map_list<Map>::type;

    namespace detail {
        struct map_keys_mapper {
            template<typename MapEntry>
            using type = typename MapEntry::key;
        };
    }

    template<typename Map>
    struct map_keys {
        using type = list_map_t<map_list_t<Map>, detail::map_keys_mapper>;
    };

    template<typename Map>
    using map_keys_t = typename map_keys<Map>::type;

    namespace detail {
        struct map_values_mapper {
            template<typename MapEntry>
            using type = typename MapEntry::value;
        };
    }

    template<typename Map>
    struct map_values {
        using type = list_map_t<map_list_t<Map>, detail::map_values_mapper>;
    };

    template<typename Map>
    using map_values_t = typename map_values<Map>::type;

    template<typename Map, typename Key>
    concept has_lookup = requires(Map map, detail::map_key_wrapper<Key> key) {
                             { map.lookup(key) };
                         };

    template<typename Map, typename Key>
    struct map_contains {
        static constexpr bool value = has_lookup<Map, Key>;
    };

    template<typename Map, typename Key>
    constexpr auto map_contains_v = map_contains<Map, Key>::value;

    template<typename Map, typename Key>
    struct map_find {
        using type = decltype(([] {
            if constexpr (has_lookup<Map, Key>) {
                return Map{}.lookup(detail::map_key_wrapper<Key>{});
            } else {
                return map_not_found{};
            }
        })());
    };

    template<typename Map, typename Key>
    using map_find_t = typename map_find<Map, Key>::type;

    // Does not overwrite
    template<typename Map, typename Key, typename Value>
    struct map_insert {};

    template<typename Map, typename Key, typename Value>
        requires(has_lookup<Map, Key>)
    struct map_insert<Map, Key, Value> {
        using type = Map;
        using result_type = map_find_t<Map, Key>;
    };

    template<typename Map, typename Key, typename Value>
        requires(!has_lookup<Map, Key>)
    struct map_insert<Map, Key, Value> {
        using type = map<list_push_back_t<map_list_t<Map>, map_entry<Key, Value>>>;
        using result_type = Value;
    };

    template<typename Map, typename Key, typename Value>
    using map_insert_t = typename map_insert<Map, Key, Value>::type;

    template<typename Map, typename Key, typename Value>
    using map_insert_result_t = typename map_insert<Map, Key, Value>::result_type;

    // Overwrites
    template<typename Map, typename Key, typename Value>
    struct map_set {};

    template<typename Map, typename Key, typename Value>
        requires(has_lookup<Map, Key>)
    struct map_set<Map, Key, Value> {
        using type = map<list_push_back_t<list_remove_t<map_list_t<Map>, map_entry<Key, map_find_t<Map, Key>>>, map_entry<Key, Value>>>;
    };

    template<typename Map, typename Key, typename Value>
        requires(!has_lookup<Map, Key>)
    struct map_set<Map, Key, Value> {
        using type = map<list_push_back_t<map_list_t<Map>, map_entry<Key, Value>>>;
    };

    template<typename Map, typename Key, typename Value>
    using map_set_t = typename map_set<Map, Key, Value>::type;
}// namespace helpers
