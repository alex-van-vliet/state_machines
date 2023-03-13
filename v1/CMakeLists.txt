add_library(v1_state_machines INTERFACE
        char_state_list_map.hpp
        dfa.hpp
        epsilon_nfa.hpp
        nfa.hpp
        regex.hpp
        runtime_nfa.hpp
        state_list.hpp
        state_list_list.hpp
        states.hpp
        string.hpp
        transition_list.hpp
        transitions.hpp
        types.hpp
        )
target_include_directories(v1_state_machines INTERFACE .)

add_executable(v1_main main.cpp)
target_link_libraries(v1_main PRIVATE v1_state_machines)
