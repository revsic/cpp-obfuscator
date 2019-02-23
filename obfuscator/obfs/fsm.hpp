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

        template <typename Event, typename... Others>
        constexpr static void run() {
            if (next<Event>::action()) {
                return;
            }
            next_s<Event>::template run<Others...>();
        }

        template <typename Event>
        constexpr static void run() {
            next<Event>::action();
        }
    };
}

#endif