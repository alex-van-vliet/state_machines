#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <stdexcept>

namespace helpers {
    // Assumes null-terminated strings
    template<std::size_t N>
    class string {
    public:
        // Must be public for use in NTTP
        std::array<char, N> chars{};

        // Not explicit as used for user-defined string literals
        constexpr string(const char (&s)[N + 1]) {
            std::copy_n(s, N, chars.begin());
        }

        constexpr size_t size() const {
            return N;
        }

        constexpr char &at(std::size_t pos) {
            if (pos >= size()) {
                throw std::out_of_range{"regex::string"};
            }
            return chars[pos];
        }

        constexpr const char &at(std::size_t pos) const {
            if (pos >= size()) {
                throw std::out_of_range{"regex::string"};
            }
            return chars[pos];
        }
    };

    template<size_t N>
    string(const char (&s)[N]) -> string<N - 1>;

    namespace literals {
        template<helpers::string Str>
        constexpr auto operator""_s() {
            return Str;
        }
    }// namespace literals
}// namespace helpers
