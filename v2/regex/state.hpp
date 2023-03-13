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

    struct state_to_id {
        template<typename Value>
        static constexpr auto value = Value::id;
    };

    struct state_equals {
        template<typename StateA, typename StateB>
        static constexpr bool value = StateA::id == StateB::id;
    };

    template<typename StateList>
    struct filter_state {
        template<typename State>
        static constexpr bool value = helpers::list_contains_v<StateList, State>;
    };
}// namespace regex
