#pragma once

#include "types.hpp"
#include <iostream>

namespace states {
    struct DrawInit {
        template<size_t STATE>
        void operator()() const {
            std::cout << "    invisible -> " << STATE << ";\n";
        }
    };
    struct DrawFinal {
        template<size_t STATE>
        void operator()() const {
            std::cout << "    " << STATE << " [shape=doublecircle];\n";
        }
    };
}// namespace states
