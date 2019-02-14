#include <string_obfs.hpp>
#include <catch2/catch.hpp>

TEST_CASE("xor string obfs", "[StringObfs]") {
    using byte = unsigned char;

    auto xor = [](byte key) constexpr {
        return [=](char c) constexpr -> char {
            return c ^ key;
        };
    };

    REQUIRE(
        make_stringobfs("Hello World !", xor(0x20), xor(0x20)).decode()
        == std::string_view("Hello World !"));
}