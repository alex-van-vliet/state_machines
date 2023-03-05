cmake_minimum_required(VERSION 3.24)
project(state_machines)

set(CMAKE_CXX_STANDARD 20)

ADD_DEFINITIONS(-DSTAGE6)

add_library(state_machines INTERFACE
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
        helpers/list.hpp
        helpers/map.hpp
        helpers/string.hpp
        regex/tokens.hpp
        regex/state_machines.hpp
        regex/state.hpp
        regex/transitions.hpp
        )
target_include_directories(state_machines INTERFACE .)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE state_machines)

add_executable(main2 main2.cpp)
target_link_libraries(main2 PRIVATE state_machines)

add_executable(list helpers/list.cpp)
target_link_libraries(list PRIVATE state_machines)

add_executable(string helpers/string.cpp)
target_link_libraries(string PRIVATE state_machines)

add_executable(map helpers/map.cpp)
target_link_libraries(map PRIVATE state_machines)
