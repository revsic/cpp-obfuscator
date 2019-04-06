#include "../obfuscator.hpp"

#include <iostream>

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
        std::cout << "Trigged" << std::endl;
    }
};
bool Action::trigged = true;

struct Dummy {
    static int dummy;
    static void dummy1() {
        dummy += 10;
        std::cout << "Dummy1" << std::endl;
    }
    static void dummy2() {
        dummy *= 20;
        std::cout << "Dummy2" << std::endl;
    }
    static void dummy3() {
        std::exit(1);
    }
};
int Dummy::dummy = 0;

int main() {
    using namespace obfs;
    using machine = StateMachine<
        Stage<state1, Next<event5 , state2, Dummy::dummy1>,
                      Next<event1 , state3, Dummy::dummy3>>,
        Stage<state2, Next<event2 , state4>>,
        Stage<state3, Next<None   , state3>>,
        Stage<state4, Next<event4 , state1>,
                      Next<event3 , state5, Dummy::dummy2>>,
        Stage<state5, Next<Trigger, Final, Action::action>>>;

    auto next1 = machine::run(state1{}, event5{});
    auto next2 = machine::run(next1, event2{});
    auto next3 = machine::run(next2, event3{});
    auto next4 = machine::run(next3, Trigger{});

    return 0;
}