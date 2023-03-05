#include "map.hpp"

template<auto Value>
struct wrapper {
    static constexpr auto value = Value;
};

using empty_map = helpers::map<helpers::list_construct_t<>>;
using my_map = helpers::map<helpers::list_construct_t<
        helpers::map_entry<int64_t, wrapper<64>>,
        helpers::map_entry<float, wrapper<32>>,
        helpers::map_entry<char, wrapper<8>>,
        helpers::map_entry<int32_t, wrapper<32>>>>;


static_assert(std::is_same_v<helpers::map_list_t<empty_map>, helpers::list_construct_t<>>);
static_assert(std::is_same_v<helpers::map_list_t<my_map>, helpers::list_construct_t<
                                                                  helpers::map_entry<int64_t, wrapper<64>>,
                                                                  helpers::map_entry<float, wrapper<32>>,
                                                                  helpers::map_entry<char, wrapper<8>>,
                                                                  helpers::map_entry<int32_t, wrapper<32>>>>);

static_assert(std::is_same_v<helpers::map_keys_t<empty_map>, helpers::list_construct_t<>>);
static_assert(std::is_same_v<helpers::map_keys_t<my_map>, helpers::list_construct_t<int64_t, float, char, int32_t>>);

static_assert(std::is_same_v<helpers::map_values_t<empty_map>, helpers::list_construct_t<>>);
static_assert(std::is_same_v<helpers::map_values_t<my_map>, helpers::list_construct_t<wrapper<64>, wrapper<32>, wrapper<8>, wrapper<32>>>);

static_assert(helpers::map_contains_v<empty_map, int64_t> == false);
static_assert(helpers::map_contains_v<my_map, int16_t> == false);
static_assert(helpers::map_contains_v<my_map, int64_t> == true);
static_assert(helpers::map_contains_v<my_map, char> == true);

static_assert(std::is_same_v<helpers::map_find_t<empty_map, int64_t>, helpers::map_not_found>);
static_assert(std::is_same_v<helpers::map_find_t<my_map, int16_t>, helpers::map_not_found>);
static_assert(std::is_same_v<helpers::map_find_t<my_map, int64_t>, wrapper<64>>);
static_assert(std::is_same_v<helpers::map_find_t<my_map, char>, wrapper<8>>);

static_assert(std::is_same_v<helpers::map_insert_t<empty_map, int64_t, wrapper<32>>, helpers::map<helpers::list_construct_t<
                                                                                             helpers::map_entry<int64_t, wrapper<32>>>>>);
static_assert(std::is_same_v<helpers::map_insert_t<my_map, int64_t, wrapper<32>>, my_map>);
static_assert(std::is_same_v<helpers::map_insert_t<my_map, int16_t, wrapper<16>>, helpers::map<helpers::list_construct_t<
                                                                                          helpers::map_entry<int64_t, wrapper<64>>,
                                                                                          helpers::map_entry<float, wrapper<32>>,
                                                                                          helpers::map_entry<char, wrapper<8>>,
                                                                                          helpers::map_entry<int32_t, wrapper<32>>,
                                                                                          helpers::map_entry<int16_t, wrapper<16>>>>>);

static_assert(std::is_same_v<helpers::map_insert_result_t<empty_map, int64_t, wrapper<32>>, wrapper<32>>);
static_assert(std::is_same_v<helpers::map_insert_result_t<my_map, int64_t, wrapper<32>>, wrapper<64>>);
static_assert(std::is_same_v<helpers::map_insert_result_t<my_map, int16_t, wrapper<16>>, wrapper<16>>);

static_assert(std::is_same_v<helpers::map_set_t<empty_map, int64_t, wrapper<32>>, helpers::map<helpers::list_construct_t<
                                                                                          helpers::map_entry<int64_t, wrapper<32>>>>>);
static_assert(std::is_same_v<helpers::map_set_t<my_map, int64_t, wrapper<32>>, helpers::map<helpers::list_construct_t<
                                                                                       helpers::map_entry<int64_t, wrapper<32>>,
                                                                                       helpers::map_entry<float, wrapper<32>>,
                                                                                       helpers::map_entry<char, wrapper<8>>,
                                                                                       helpers::map_entry<int32_t, wrapper<32>>>>>);
static_assert(std::is_same_v<helpers::map_set_t<my_map, int16_t, wrapper<16>>, helpers::map<helpers::list_construct_t<
                                                                                       helpers::map_entry<int64_t, wrapper<64>>,
                                                                                       helpers::map_entry<float, wrapper<32>>,
                                                                                       helpers::map_entry<char, wrapper<8>>,
                                                                                       helpers::map_entry<int32_t, wrapper<32>>,
                                                                                       helpers::map_entry<int16_t, wrapper<16>>>>>);

int main() {}
