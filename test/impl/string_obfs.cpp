#include <string_obfs.hpp>
#include <catch2/catch.hpp>

template <char Key>
constexpr char xor(char value) {
    return value ^ Key;
}

TEST_CASE("xor test", "[obfs::String]") {
    REQUIRE(
        obfs::make_string<xor<0x50>, xor<0x50>>("Hello World !").decode()
        == std::string_view("Hello World !"));
}