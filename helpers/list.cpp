#include "list.hpp"

#include <iostream>

using empty_list = helpers::list<>;
using my_list = helpers::list<int64_t, float, char, int32_t>;

static_assert(helpers::list_length_v<empty_list> == 0);
static_assert(helpers::list_length_v<my_list> == 4);

static_assert(helpers::list_contains_v<empty_list, int64_t> == false);
static_assert(helpers::list_contains_v<my_list, int64_t> == true);
static_assert(helpers::list_contains_v<my_list, bool> == false);

static_assert(std::is_same_v<helpers::list_concat_t<my_list, helpers::list<bool, float>>, helpers::list<int64_t, float, char, int32_t, bool, float>>);
static_assert(std::is_same_v<helpers::list_concat_t<my_list, helpers::list<bool, float>, helpers::list<int16_t>>, helpers::list<int64_t, float, char, int32_t, bool, float, int16_t>>);
static_assert(std::is_same_v<helpers::list_concat_t<empty_list, my_list, empty_list, helpers::list<bool, float>, empty_list>, helpers::list<int64_t, float, char, int32_t, bool, float>>);

static_assert(std::is_same_v<helpers::list_push_back_t<empty_list, bool>, helpers::list<bool>>);
static_assert(std::is_same_v<helpers::list_push_back_t<my_list, bool>, helpers::list<int64_t, float, char, int32_t, bool>>);

static_assert(std::is_same_v<helpers::list_push_front_t<empty_list, bool>, helpers::list<bool>>);
static_assert(std::is_same_v<helpers::list_push_front_t<my_list, bool>, helpers::list<bool, int64_t, float, char, int32_t>>);

template<auto Value>
struct wrapper {};

struct size_of {
    template<typename Value>
    using type = wrapper<sizeof(Value)>;
};

static_assert(std::is_same_v<helpers::list_map_t<empty_list, size_of>, helpers::list<>>);
static_assert(std::is_same_v<helpers::list_map_t<my_list, size_of>, helpers::list<wrapper<sizeof(int64_t)>, wrapper<sizeof(float)>, wrapper<sizeof(char)>, wrapper<sizeof(int32_t)>>>);

struct int32_or_int64 {
    template<typename Value>
    static constexpr bool value = std::is_same_v<Value, int64_t> || std::is_same_v<Value, int32_t>;
};

static_assert(std::is_same_v<helpers::list_filter_t<empty_list, int32_or_int64>, helpers::list<>>);
static_assert(std::is_same_v<helpers::list_filter_t<my_list, int32_or_int64>, helpers::list<int64_t, int32_t>>);

static_assert(std::is_same_v<helpers::list_find_t<empty_list, int32_or_int64>, helpers::list_not_found>);
static_assert(std::is_same_v<helpers::list_find_t<my_list, int32_or_int64>, int64_t>);

struct key {
    template<typename Value>
    static constexpr int value = sizeof(Value);
};

static_assert(std::is_same_v<helpers::list_sort_t<empty_list, key>, helpers::list<>>);
static_assert(std::is_same_v<helpers::list_sort_t<my_list, key>, helpers::list<char, float, int32_t, int64_t>>);

struct is_same {
    template<typename ValueA, typename ValueB>
    static constexpr bool value = std::is_same_v<ValueA, ValueB>;
};
static_assert(std::is_same_v<helpers::list_unique_t<empty_list, is_same>, helpers::list<>>);
static_assert(std::is_same_v<helpers::list_unique_t<my_list, is_same>, helpers::list<int64_t, float, char, int32_t>>);
static_assert(std::is_same_v<helpers::list_unique_t<helpers::list<int, int, int>, is_same>, helpers::list<int>>);
static_assert(std::is_same_v<helpers::list_unique_t<helpers::list<int, int, float, float, int, int, int>, is_same>, helpers::list<int, float, int>>);


struct Printer {
    void operator()(int32_t) {
        std::cout << "int 32" << std::endl;
    }

    void operator()(int64_t) {
        std::cout << "int 64" << std::endl;
    }

    void operator()(float) {
        std::cout << "float" << std::endl;
    }

    void operator()(char) {
        std::cout << "char" << std::endl;
    }
};

int main() {
    helpers::list_apply<my_list, Printer>{}();
}
