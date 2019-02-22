#include "../obfuscator.hpp"

#include <iostream>

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

int main() {
    using table = obfs::make_table<
        obfs::encoder_seq<xor_<0x50>, add<10>, comp<xor_<0x50>, add<10>>>,
        obfs::decoder_seq<xor_<0x50>, add<-10>, comp<add<-10>, xor_<0x50>>>>;

    // constexpr auto str = obfs::make_string<table>("Hello World !");
    // std::cout << str.decode() << std::endl;

    std::cout << MAKE_STRING(table, "Hello World !") << std::endl;
}