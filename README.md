# cpp-obfuscator
[![License](https://img.shields.io/badge/Licence-MIT-blue.svg)](https://github.com/revsic/cpp-obfuscator/blob/master/LICENSE)
[![Build Status](https://dev.azure.com/revsic99/cpp-obfuscator/_apis/build/status/revsic.cpp-obfuscator?branchName=master)](https://dev.azure.com/revsic99/cpp-obfuscator/_build/latest?definitionId=3&branchName=master)

C++ implementation of compile time obfuscator

## Compiler Support
| Compiler | Version |
| -- | -- |
| Visual Studio | 2019 |
| Clang++ | 8.0 |
| g++ | 7.3 |
| Apple Clang | Mojave |

## Usage

Use existing single header [obfuscator.hpp](./obfuscator.hpp) or run [script](./script/merge.py) to merge multiple headers in directory [obfuscator](./obfuscator).

```bash
python -m script.merge
```

Include [obfuscator.hpp](./obfuscator.hpp) to use it.
```C++
#include "../obfuscator.hpp"

template <char key>
constexpr char xor_(char c) {
    return c ^ key;
}

std::cout << obfs::make_string<xor_<0x50>, xor_<0x50>>("Hello World !\n").decode();
```

## String

Compile time string obfuscator. No more raw string in binary, sample [string_obfuscator](./sample/string_obfs.cpp).

0. Sample encoder, decoder
```C++
template <char key>
constexpr char xor_(char c) {
    return c ^ key;
}

template <char Key>
constexpr char add(char c) {
    return c + Key;
}

template <char(*f)(char), char(*g)(char)>
constexpr char comp(char c) {
    return f(g(c));
}
```

1. Single encoder, decoder.
```C++
std::cout << obfs::make_string<xor_<0x50>, xor_<0x50>>("Hello World !\n").decode();
```

2. Multiple encoder, decoder and random selection.
```C++
using table = obfs::make_table<
    obfs::encoder_seq<xor_<0x50>, add<10>, comp<xor_<0x50>, add<10>>>,
    obfs::decoder_seq<xor_<0x50>, add<-10>, comp<add<-10>, xor_<0x50>>>>;

std::cout << obfs::make_string<table>("Hello World !\n").decode();
```

3. Multiple encoder, decoder as pair.
```C++
using table = obfs::make_pair_table<
    obfs::encoder_pair<xor_<0x50>, xor_<0x50>>,
    obfs::encoder_pair<add<10>, add<-10>>,
    obfs::encoder_pair<comp<xor_<0x50>, add<10>>, comp<add<-10>, xor_<0x50>>>
>;

std::cout << obfs::make_string<table>("Hello World !\n").decode();
```

4. String obfuscator macro.
```C++
MAKE_STRING(str, "Hello World !\n", table);
std::cout << str.decode();
```

## Finite state machine

Compile time finite state machine based routine obfuscator, sample [state_machine](./sample/state_machine.cpp).

0. Make state table.
```C++
using namespace obfs;
using machine = StateMachine<
    Stage<state1, Next<event5 , state2, Dummy::dummy1>,
                  Next<event1 , state3, Dummy::dummy3>>,
    Stage<state2, Next<event2 , state4>>,
    Stage<state3, Next<None   , state3>>,
    Stage<state4, Next<event4 , state1>,
                  Next<event3 , state5, Dummy::dummy2>>,
    Stage<state5, Next<Trigger, Final, Action::action>>>;
```

1. Run state machine, each execution returns next state.
```C++
auto next1 = machine::run(state1{}, event5{}); // dummy1 executed
auto next2 = machine::run(next1, event2{});
auto next3 = machine::run(next2, event3{});    // dummy2 executed
auto next4 = machine::run(next3, Trigger{});   // action executed
```