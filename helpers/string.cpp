#include "string.hpp"

using namespace helpers::literals;

constexpr auto my_string = "hello"_s;

static_assert(my_string.size() == 5);
static_assert(my_string.at(0) == 'h');
static_assert(my_string.at(1) == 'e');
static_assert(my_string.at(2) == 'l');
static_assert(my_string.at(3) == 'l');
static_assert(my_string.at(4) == 'o');

int main() {}
