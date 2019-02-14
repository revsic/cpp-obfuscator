#include <string_obfs.hpp>
#include <catch2/catch.hpp>

TEST_CASE("xor test", "[obfs::String]") {
    auto xor = [](char key) constexpr {
        return [=](char chr) constexpr {
            return static_cast<char>(key ^ chr);
        };
    };

    REQUIRE(
        obfs::make_string("Hello World !", xor(0x20), xor(0x20)).decode()
        == std::string_view("Hello World !"));
}