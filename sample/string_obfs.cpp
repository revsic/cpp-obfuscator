#include "../obfuscator/obfuscator.hpp"

#include <iostream>

template <char key>
constexpr char xor(char c) {
    return c ^ key;
}

int main() {
    constexpr auto str = obfs::make_string<xor<0x20>, xor<0x20>>("Hello World !");
    std::cout << str.decode() << std::endl;
}