add_library(v2_state_machines INTERFACE
        helpers/list.hpp
        helpers/map.hpp
        helpers/string.hpp
        regex/tokens.hpp
        regex/state_machines.hpp
        regex/state.hpp
        regex/transitions.hpp)
target_include_directories(v2_state_machines INTERFACE .)

add_executable(v2_list helpers/list.cpp)
target_link_libraries(v2_list PRIVATE v2_state_machines)

add_executable(v2_string helpers/string.cpp)
target_link_libraries(v2_string PRIVATE v2_state_machines)

add_executable(v2_map helpers/map.cpp)
target_link_libraries(v2_map PRIVATE v2_state_machines)

add_executable(v2_main main.cpp)
target_link_libraries(v2_main PRIVATE v2_state_machines)
