#include <obfs/string.hpp>
#include <catch2/catch.hpp>

template <char Key>
constexpr char enc_xor(char value) {
    return value ^ Key;
}

template <char Key>
constexpr char add(char c) {
    return c + Key;
}

template <char(*f)(char), char(*g)(char)>
constexpr char comp(char c) {
    return f(g(c));
}

TEST_CASE("Single encoder, decoder", "[obfs::String]") {
    REQUIRE(
        obfs::make_string<enc_xor<0x50>, enc_xor<0x50>>("Hello World !").decode()
        == std::string_view("Hello World !"));
}

TEST_CASE("Multiple encoder, decoder", "[obfs::String]") {
    using table = obfs::make_table<
        obfs::encoder_seq<enc_xor<0x50>, add<10>, comp<enc_xor<0x50>, add<10>>>,
        obfs::decoder_seq<enc_xor<0x50>, add<-10>, comp<add<-10>, enc_xor<0x50>>>>;

    MAKE_STRING(str, "Hello World !", table);
    REQUIRE(str.decode() == std::string_view("Hello World !"));
}

TEST_CASE("Multiple encoder, decoder pair", "[obfs::String]") {
    using table = obfs::make_pair_table<
        obfs::encoder_pair<enc_xor<0x50>, enc_xor<0x50>>,
        obfs::encoder_pair<add<10>, add<-10>>,
        obfs::encoder_pair<comp<enc_xor<0x50>, add<10>>, comp<add<-10>, enc_xor<0x50>>>
    >;

    MAKE_STRING(str, "Hello World !", table);
    REQUIRE(str.decode() == std::string_view("Hello World !"));
}
