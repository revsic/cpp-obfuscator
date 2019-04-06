#ifndef OBFS_FINITE_STATE_MACHINE
#define OBFS_FINITE_STATE_MACHINE

#include "sequence.hpp"

#include <type_traits>

namespace obfs {
    void FreeAction() {}

    template <typename Event, typename State, void(*Action)() = FreeAction>
    struct Next {
        using event = Event;
        using state = State;
        constexpr static void(*action)() = Action;
    };

    struct None {};

    template <typename State, typename... Nexts>
    struct Stage {
        using state = State;

        template <typename NextInfo, typename Event>
        using act = std::conditional_t<
            std::is_same_v<typename NextInfo::event, Event>, NextInfo, Pass>;

        template <typename Event>
        using next = typename First<None, act<Nexts, Event>...>::type;
    };

    template <typename Stage_, typename Event>
    struct next_stage {
        using type = typename Stage_::template next<Event>;
    };

    template <typename Event>
    struct next_stage<None, Event> {
        using type = None;
    };

    template <typename State>
    struct action_invoker {
        static auto action() {
            State::action();
            return typename State::state{};
        }
    };

    template <>
    struct action_invoker<None> {
        static auto action() {
            return None{};
        }
    };

    template <typename... Specs>
    struct StateMachine {
        template <typename State, typename StageT>
        using filter = std::conditional_t<
            std::is_same_v<typename StageT::state, State>, StageT, Pass>;

        template <typename State>
        using find = typename First<None, filter<State, Specs>...>::type;

        template <typename State, typename Event>
        using next_t = typename next_stage<find<State>, Event>::type;

        template <typename State, typename Event>
        static auto run(State state, Event event) {
            using next_state = next_t<std::decay_t<State>, std::decay_t<Event>>;
            return action_invoker<next_state>::action();
        }
    };
}

#endif