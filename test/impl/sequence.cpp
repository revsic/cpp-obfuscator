#include <obfs/sequence.hpp>
#include <catch2/catch.hpp>

TEST_CASE("TypeVal", "[obfs::Sequence]") {
    using val = obfs::TypeVal<int, 10>;
    
    static_assert(std::is_same_v<int, typename val::value_type>);
    static_assert(val::value == 10);
}

TEST_CASE("Sequence", "[obfs::Sequence]") {
    using seq = obfs::Sequence<int, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9>;

    static_assert(std::is_same_v<int, typename seq::value::value_type>);
    static_assert(seq::size == 10);
    static_assert(seq::index<0>::value == 0);
    static_assert(seq::index<5>::value == 5);
    static_assert(seq::index<9>::value == 9);
    static_assert(std::is_same_v<seq::index<10>, obfs::Nothing>);
}

TEST_CASE("TypeSeq", "[obfs::Sequence]") {
    using namespace obfs;
    using seq = TypeSeq<TypeVal<int, 0>, TypeVal<int, 1>, TypeVal<int, 2>, TypeVal<int, 3>>;
    
    static_assert(seq::size == 4);
    static_assert(seq::index<0>::value == 0);
    static_assert(seq::index<2>::value == 2);
    static_assert(seq::index<3>::value == 3);
    static_assert(std::is_same_v<seq::index<4>, Nothing>);
}

TEST_CASE("MinVal", "[obfs::Sequence]") {
    using min = obfs::MinVal<1, 3, 2, 4, 9, 5, 6, 0, 7, 8>;
    static_assert(min::value == 0);
}

TEST_CASE("SeqPack", "[obfs::Sequence]") {
    using pack = obfs::SeqPack<
        obfs::Sequence<int, 0, 1, 2, 3, 4>,
        obfs::Sequence<int, 1, 2, 3, 4, 5, 6>,
        obfs::Sequence<int, 2, 3, 4, 5>
    >;

    static_assert(pack::size == 4);

    using item = pack::index<2>;
    static_assert(item::size == 3);
    static_assert(item::index<0>::value == 2);
    static_assert(item::index<1>::value == 3);
    static_assert(item::index<2>::value == 4);
}

template <typename Val>
using condition = std::conditional_t<(Val::value > 3), Val, obfs::Pass>;

template <typename... Values>
using first = typename obfs::First<obfs::Nothing, condition<Values>...>::type;

TEST_CASE("First", "[obfs::Sequence]") {
    using result = first<
        obfs::TypeVal<int, 0>,
        obfs::TypeVal<int, 3>,
        obfs::TypeVal<int, 2>,
        obfs::TypeVal<int, 5>,
        obfs::TypeVal<int, 1>,
        obfs::TypeVal<int, 4>>;

    static_assert(result::value == 5);
}