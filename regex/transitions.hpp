#pragma once

namespace regex {
    struct epsilon_transition {
    };

    template<char Value>
    struct character_transition {
        constexpr static char value = Value;
    };
}// namespace regex
