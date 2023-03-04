#pragma once

#include <cstddef>
#include <iostream>

namespace regex {
    template<std::size_t Id>
    struct state {
        static constexpr auto id = Id;
    };

    struct draw_init_state {
        template<size_t Id>
        void operator()(state<Id>) const {
            std::cout << "    invisible -> " << Id << ";\n";
        }
    };

    struct draw_final_state {
        template<size_t Id>
        void operator()(state<Id>) const {
            std::cout << "    " << Id << " [shape=doublecircle];\n";
        }
    };
}// namespace regex
