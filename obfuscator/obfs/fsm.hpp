#ifndef OBFS_FINITE_STATE_MACHINE
#define OBFS_FINITE_STATE_MACHINE

#include <type_traits>

namespace obfs {
    template <typename Event, typename State>
    struct Next {
        using event = Event;
        using state = State;
    };

    struct FalseType {};

    template <typename IfFalse, typename T, typename... Ts>
    struct First {
        using type = std::conditional_t<
            std::is_same_v<T, FalseType>,
            typename First<IfFalse, Ts...>::type,
            T>;
    };

    template <typename IfFalse, typename T>
    struct First<IfFalse, T> {
        using type = std::conditional_t<
            std::is_same_v<T, FalseType>,
            IfFalse,
            T>;
    };

    template <typename State, typename... Nexts>
    struct Stage {

        template <typename Cond, typename Event>
        using act = std::conditional_t<
            std::is_same_v<typename Cond::event, Event>,
            typename Cond::state,
            FalseType>;

        template <typename Event>
        using next = typename First<State, act<Nexts, Event>...>::type;
    };

    template <typename... Specs>
    struct StateMachine {
        
    };

#define STATE(Name) struct Name {};
#define EVENT(Name) struct Name {};

    STATE(Final);

    EVENT(none);
    EVENT(Trigger);

    STATE(state1); STATE(state2); STATE(state3); STATE(state4); STATE(state5);
    EVENT(event1); EVENT(event2); EVENT(event3); EVENT(event4); EVENT(event5);

    using machine = StateMachine<
        Stage<state1, Next<event5, state2>,
                      Next<event1, state3>>,
        Stage<state2, Next<event2, state4>>,
        Stage<state3, Next<none, state3>>,
        Stage<state4, Next<event4, state1>,
                      Next<event3, state5>>,
        Stage<state5, Next<Trigger, Final>>>;

}

#endif