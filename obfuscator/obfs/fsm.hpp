#ifndef OBFS_FINITE_STATE_MACHINE
#define OBFS_FINITE_STATE_MACHINE

#include <type_traits>

namespace obfs {
    constexpr bool FreeAction() {
        return false;
    }

    template <typename Event, typename State, bool(*Action)() = FreeAction>
    struct Next {
        using event = Event;
        using state = State;
        constexpr static bool(*action)() = Action;
    };

    struct Pass {};

    template <typename IfAllPass, typename T, typename... Ts>
    struct First {
        using type = std::conditional_t<
            std::is_same_v<T, Pass>,
            typename First<IfAllPass, Ts...>::type,
            T>;
    };

    template <typename IfAllPass, typename T>
    struct First<IfAllPass, T> {
        using type = std::conditional_t<
            std::is_same_v<T, Pass>,
            IfAllPass,
            T>;
    };

    struct None {};

    template <typename State, typename... Nexts>
    struct Stage {
        using state = State;

        template <typename Cond, typename Event>
        using act = std::conditional_t<
            std::is_same_v<typename Cond::event, Event>, Cond, Pass>;

        template <typename Event>
        using next = typename First<Next<None, State>, act<Nexts, Event>...>::type;
    };

    template <typename Machine, typename... Events>
    struct MachineExecutor;

    template <typename Machine, typename Event, typename... Others>
    struct MachineExecutor<Machine, Event, Others...> {
        using next = typename Machine::template next<Event>;
        using next_s = typename Machine::template next_s<Event>;

        constexpr static void run() {
            if (next::action()) {
                return;
            }
            MachineExecutor<next_s, Others...>::run();
        }
    };

    template <typename Machine, typename Event>
    struct MachineExecutor<Machine, Event> {
        using next = typename Machine::template next<Event>;
        constexpr static void run() {
            next::action();
        }
    };

    template <typename State, typename... Specs>
    struct StateMachine {
        template <typename ST>
        using find = std::conditional_t<
            std::is_same_v<typename ST::state, State>, ST, Pass>;

        using stage = typename First<Pass, find<Specs>...>::type;

        template <typename Event>
        using next = typename stage::template next<Event>;

        template <typename Event>
        using next_s = StateMachine<typename next<Event>::state, Specs...>;

        template <typename... Events>
        constexpr static void run() {
            using self = StateMachine<State, Specs...>;
            MachineExecutor<self, Events...>::run();
        }
    };
}

#endif