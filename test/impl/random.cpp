#include <obfs/random.hpp>
#include <catch2/catch.hpp>

using uint64_t = unsigned long long;

// https://en.wikipedia.org/wiki/Xorshift
uint64_t xorshift128plus(uint64_t state[2]) {
	uint64_t t = state[0];
	uint64_t const s = state[1];
	state[0] = s;
	t ^= t << 23;		// a
	t ^= t >> 17;		// b
	t ^= s ^ (s >> 26);	// c
	state[1] = t;
	return t + s;
}

TEST_CASE("Random case", "[obfs::Xorshift+]") {
    constexpr size_t val = MAKE_RAND_VAL(100);
    uint64_t state[2] = { val, val << 1 };

    REQUIRE(obfs::Xorshiftplus<val, 1>::value == xorshift128plus(state));
    REQUIRE(obfs::Xorshiftplus<val, 2>::value == xorshift128plus(state));
    REQUIRE(obfs::Xorshiftplus<val, 3>::value == xorshift128plus(state));
}
