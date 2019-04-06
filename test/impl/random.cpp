#include <obfs/random.hpp>
#include <catch2/catch.hpp>

using ull = unsigned long long;

// https://en.wikipedia.org/wiki/Xorshift
ull xorshift128plus(ull state[2]) {
	ull t = state[0];
	ull const s = state[1];
	state[0] = s;
	t ^= t << 23;		// a
	t ^= t >> 17;		// b
	t ^= s ^ (s >> 26);	// c
	state[1] = t;
	return t + s;
}

TEST_CASE("Random case", "[obfs::Xorshift+]") {
    constexpr size_t val = MAKE_RAND_VAL(100);
    ull state[2] = { val, val << 1 };

    REQUIRE(obfs::Xorshiftplus<val, 1>::value == xorshift128plus(state));
    REQUIRE(obfs::Xorshiftplus<val, 2>::value == xorshift128plus(state));
    REQUIRE(obfs::Xorshiftplus<val, 3>::value == xorshift128plus(state));
}
