#include <obfs/string.hpp>
#include <catch2/catch.hpp>

template <char Key>
constexpr char enc_xor(char value) {
    return value ^ Key;
}

TEST_CASE("xor test", "[obfs::String]") {
    REQUIRE(
        obfs::make_string<enc_xor<0x50>, enc_xor<0x50>>("Hello World !").decode()
        == std::string_view("Hello World !"));
}