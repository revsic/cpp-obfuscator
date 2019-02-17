#include "../obfuscator.hpp"

#include <iostream>

template <char key>
constexpr char enc_xor(char c) {
    return c ^ key;
}

int main() {
    constexpr auto str = obfs::make_string<enc_xor<0x50>, enc_xor<0x50>>("Hello World !");
    std::cout << str.decode() << std::endl;
}