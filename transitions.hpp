#pragma once

#include <iostream>

namespace transitions {
    struct Epsilon {
    };

    template<char C>
    struct Character {
        constexpr static char c = C;
    };

    struct Draw {
        template<typename TRANSITION, size_t FROM, size_t TO>
        struct Helper {
            void operator()() const {
                std::cout << "    " << FROM << " -> " << TO << ";\n";
            }
        };
        template<char c, size_t FROM, size_t TO>
        struct Helper<Character<c>, FROM, TO> {
            void operator()() const {
                std::cout << "    " << FROM << " -> " << TO << "[label=" << c << "];\n";
            }
        };

        template<typename TRANSITION, size_t FROM, size_t TO>
        void operator()() const {
            Helper<TRANSITION, FROM, TO>{}();
        }
    };
}// namespace transitions
