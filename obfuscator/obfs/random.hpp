#ifndef COMPILE_TIME_RANDOM
#define COMPILE_TIME_RANDOM

namespace obfs {
    constexpr char TIME[] = __TIME__;
    constexpr int digit(char c) {
        return c - '0';
    }
    constexpr int SEED = digit(TIME[7]) +
                         digit(TIME[6]) * 10 +
                         digit(TIME[4]) * 60 +
                         digit(TIME[3]) * 600 +
                         digit(TIME[1]) * 3600 +
                         digit(TIME[0]) * 36000;

    using size_t = decltype(sizeof(void*));

    template <size_t Idx>
    struct Xorshiftplus {
        using prev = Xorshiftplus<Idx - 1>;

        constexpr static size_t update() {
            constexpr size_t x = prev::state0 ^ (prev::state0 << 23);
            constexpr size_t y = prev::state1;
            return x ^ y ^ (x >> 17) ^ (y >> 26);
        }

        constexpr static size_t state0 = prev::state1;
        constexpr static size_t state1 = update();

        constexpr static size_t value = state0 + state1;
    };

    template <>
    struct Xorshiftplus<0> {
        constexpr static size_t state0 = static_cast<size_t>(SEED);
        constexpr static size_t state1 = static_cast<size_t>(SEED << 1);
        constexpr static size_t value = state0 + state1;
    };

    template <size_t Idx, size_t Mod>
    constexpr size_t RAND_VAL = Xorshiftplus<Idx>::value % Mod;
}

#define MAKE_RAND_VAL(MOD) RAND_VAL<__LINE__, MOD>

#endif