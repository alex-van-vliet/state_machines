#pragma once

#include "state_list.hpp"
#include "transition_list.hpp"
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace runtime_nfa {
    template<typename STATE_LIST>
    constexpr void StateListToUnorderedSet(std::unordered_set<size_t> &v) {
        v.insert(STATE_LIST::state);
        StateListToUnorderedSet<typename STATE_LIST::next>(v);
    }

    template<>
    constexpr void StateListToUnorderedSet<::state_list::End>(std::unordered_set<size_t> &) {
    }
    template<typename TRANSITION_LIST>
    constexpr void TransitionListToUnorderedMapVector(std::vector<std::unordered_map<char, size_t>> &v) {
        v[TRANSITION_LIST::entry::from].emplace(TRANSITION_LIST::entry::transition::c, TRANSITION_LIST::entry::to);
        TransitionListToUnorderedMapVector<typename TRANSITION_LIST::next>(v);
    }

    template<>
    constexpr void TransitionListToUnorderedMapVector<::transition_list::End>(std::vector<std::unordered_map<char, size_t>> &) {
    }


    template<typename DFA>
    struct ToRuntimeT {
        size_t init_state;
        std::unordered_set<size_t> final_states;
        std::vector<std::unordered_map<char, size_t>> transitions;

        ToRuntimeT()
            : init_state{DFA::init_state}, final_states{}, transitions(DFA::states_count) {
            StateListToUnorderedSet<typename DFA::final_state_list>(final_states);
            TransitionListToUnorderedMapVector<typename DFA::transition_list>(transitions);
        }

        bool test(std::string_view v) {
            size_t current_state = init_state;
            for (char c: v) {
                auto next_it = transitions[current_state].find(c);
                if (next_it == transitions[current_state].end()) {
                    return false;
                }
                current_state = next_it->second;
            }
            return final_states.contains(current_state);
        }
    };
}// namespace runtime_nfa
