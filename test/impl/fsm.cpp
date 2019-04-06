#include <obfs/fsm.hpp>
#include <catch2/catch.hpp>

#define STATE(Name) struct Name {};
#define EVENT(Name) struct Name {};

STATE(Final);
EVENT(Trigger);

STATE(state1); STATE(state2); STATE(state3); STATE(state4); STATE(state5);
EVENT(event1); EVENT(event2); EVENT(event3); EVENT(event4); EVENT(event5);

struct Action {
    static bool trigged;
    static void action() {
        trigged = true;
    }
};
bool Action::trigged = true;

struct Dummy {
    static int dummy;
    static void dummy1() {
        dummy += 10;
    }
    static void dummy2() {
        dummy *= 20;
    }
};
int Dummy::dummy = 0;

TEST_CASE("Next", "[obfs::StateMachine]") {
    using next = obfs::Next<event1, state1>;
    static_assert(std::is_same_v<event1, typename next::event>);
    static_assert(std::is_same_v<state1, typename next::state>);
    static_assert(next::action == obfs::FreeAction);

    using next2 = obfs::Next<event2, state2, Action::action>;
    static_assert(next2::action == Action::action);
}

TEST_CASE("Stage", "[obfs::StateMachine]") {
    using stage = obfs::Stage<
        state1,
        obfs::Next<event5, state2, Dummy::dummy1>,
        obfs::Next<event1, state3>>;
    
    static_assert(std::is_same_v<
        obfs::Next<event5, state2, Dummy::dummy1>,
        typename stage::template next<event5>>);

    static_assert(std::is_same_v<
        obfs::Next<event5, state2, Dummy::dummy1>,
        typename obfs::next_stage<stage, event5>::type>);

    static_assert(std::is_same_v<
        obfs::Next<event1, state3>,
        typename stage::template next<event1>>);

    static_assert(std::is_same_v<
        obfs::Next<event1, state3>,
        typename obfs::next_stage<stage, event1>::type>);

    static_assert(std::is_same_v<obfs::None, typename stage::template next<event3>>);

    static_assert(std::is_same_v<obfs::None, typename obfs::next_stage<stage, event3>::type>);

    static_assert(std::is_same_v<obfs::None, typename obfs::next_stage<obfs::None, event1>::type>);
}

TEST_CASE("StateMachine", "[obfs::StateMachine]") {
    using namespace obfs;
    using machine = StateMachine<
        Stage<state1, Next<event5 , state2, Dummy::dummy1>,
                      Next<event1 , state3>>,
        Stage<state2, Next<event2 , state4>>,
        Stage<state3, Next<None   , state3>>,
        Stage<state4, Next<event4 , state1>,
                      Next<event3 , state5, Dummy::dummy2>>,
        Stage<state5, Next<Trigger, Final, Action::action>>>;

    auto failure = machine::run(obfs::None{}, event5{});
    static_assert(std::is_same_v<decltype(failure), obfs::None>);

    auto next1 = machine::run(state1{}, event5{});
    static_assert(std::is_same_v<decltype(next1), state2>);
    REQUIRE(Dummy::dummy == 10);

    auto next2 = machine::run(next1, event2{});
    static_assert(std::is_same_v<decltype(next2), state4>);

    auto failure2 = machine::run(next2, event1{});
    static_assert(std::is_same_v<decltype(failure2), obfs::None>);
    
    auto next3 = machine::run(next2, event3{});
    static_assert(std::is_same_v<decltype(next3), state5>);
    REQUIRE(Dummy::dummy == 200);

    auto next4 = machine::run(next3, Trigger{});
    static_assert(std::is_same_v<decltype(next4), Final>);
    REQUIRE(Action::trigged);
}
