cmake_minimum_required(VERSION 3.24)
project(state_machines)

set(CMAKE_CXX_STANDARD 20)

add_executable(state_machines
        main.cpp
        epsilon_nfa.hpp
        states.hpp
        transition_list.hpp
        transitions.hpp
        types.hpp
        )
