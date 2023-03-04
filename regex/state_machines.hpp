#pragma once

#include <cstddef>
#include <iostream>

#include "transitions.hpp"

namespace regex {
    template<typename FromState, typename ToState, typename Transition>
    struct transition_entry {
        using from_state = FromState;
        using to_state = ToState;
        using transition = Transition;
    };

    struct draw_transition_entry {
        template<size_t FromState, size_t ToState>
        void operator()(transition_entry<state<FromState>, state<ToState>, epsilon_transition>) const {
            std::cout << "    " << FromState << " -> " << ToState << ";\n";
        }

        template<size_t FromState, size_t ToState, char Value>
        void operator()(transition_entry<state<FromState>, state<ToState>, character_transition<Value>>) const {
            std::cout << "    " << FromState << " -> " << ToState << " [label=" << Value << "];\n";
        }
    };

    template<std::size_t StateCount, typename InitState, typename FinalState, typename TransitionList>
    struct epsilon_nfa {
        static constexpr auto state_count = StateCount;
        using init_state = InitState;
        using final_state = FinalState;
        using transition_list = TransitionList;

        void draw() const {
            std::cout << "digraph {\n";
            std::cout << "    invisible [label=\"\", shape=none];\n";
            for (size_t i = 0; i < state_count; ++i) {
                std::cout << "    " << i << ";\n";
            }
            draw_final_state{}(final_state{});
            draw_init_state{}(init_state{});
            helpers::list_apply<transition_list, draw_transition_entry>{}();
            std::cout << "}" << std::endl;
        }
    };

    template<size_t Offset>
    struct offset_transition {
        template<typename TransitionEntry>
        using type = transition_entry<state<Offset + TransitionEntry::from_state::id>,
                                      state<Offset + TransitionEntry::to_state::id>,
                                      typename TransitionEntry::transition>;
    };
}// namespace regex
