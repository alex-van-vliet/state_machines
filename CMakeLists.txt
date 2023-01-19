cmake_minimum_required(VERSION 3.24)
project(state_machines)

set(CMAKE_CXX_STANDARD 20)

add_executable(state_machines
        main.cpp
        char_state_list_map.hpp
        dfa.hpp
        epsilon_nfa.hpp
        nfa.hpp
        runtime_nfa.hpp
        state_list.hpp
        state_list_list.hpp
        states.hpp
        transition_list.hpp
        transitions.hpp
        types.hpp
        )
