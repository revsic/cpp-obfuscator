#ifndef OBFUSCATOR_HPP
#define OBFUSCATOR_HPP

#include <type_traits>
#include <utility>

#define COMPILE_TIME_RANDOM
#define MAKE_RAND_VAL(MOD) RAND_VAL<__LINE__, MOD>
#define COMPILE_TIME_SEQUENCE
#define OBFS_STRING
#define MAKE_STRING(Var, Table, String) constexpr auto Var = obfs::make_string<Table>(String);


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


namespace obfs {
    template <typename T, T Val>
    struct TypeVal {
        using value_type = T;
        constexpr static T value = Val;
    };

    struct Nothing {};

    template <typename T, T Val, T... Others>
    struct Sequence {
        using value = TypeVal<T, Val>;
        using next = Sequence<T, Others...>;

        template <std::size_t Idx>
        using index = std::conditional_t<Idx == 0, value, typename next::template index<Idx - 1>>;
    };

    template <typename T, T Val>
    struct Sequence<T, Val> {
        using value = TypeVal<T, Val>;

        template <std::size_t Idx>
        using index = std::conditional_t<Idx == 0, value, Nothing>;
    };

    template <typename T, typename... Ts>
    struct TypeSeq {
        using type = T;
        using next = TypeSeq<Ts...>;

        template <std::size_t Idx>
        using index = std::conditional_t<Idx == 0, type, typename next::template index<Idx - 1>>;
    };

    template <typename T>
    struct TypeSeq<T> {
        using type = T;

        template <std::size_t Idx>
        using index = std::conditional_t<Idx == 0, type, Nothing>;
    };

    template <typename... T>
    struct SeqPack {
        constexpr static std::size_t size = sizeof...(T);

        template <std::size_t Idx, typename U>
        using getter = typename U::template index<Idx>;

        template <std::size_t Idx>
        using index = TypeSeq<getter<Idx, T>...>;
    };
}


namespace obfs {
    using Encoder = char(*)(char);
    using Decoder = char(*)(char);

    template <std::size_t size, Encoder encoder, Decoder decoder>
    class String {
    public:
        template <std::size_t... Idx>
        constexpr String(char const* str,
                         std::index_sequence<Idx...>):
            str{ encoder(str[Idx])... } {
            // Do Nothing
        }

        inline char const* decode() const {
            for (char& chr : str) {
                chr = decoder(chr);
            }
            return str;
        }

    private:
        mutable char str[size];
    };

    template <Encoder encoder, Decoder decoder, std::size_t size>
    constexpr auto make_string(char const (&str)[size]) {
        return String<size, encoder, decoder>(str, std::make_index_sequence<size>());
    }

    template <Encoder... encoders>
    using encoder_seq = Sequence<Encoder, encoders...>;

    template <Decoder... decoders>
    using decoder_seq = Sequence<Decoder, decoders...>;

    template <typename EncoderSeq, typename DecoderSeq>
    using make_table = SeqPack<EncoderSeq, DecoderSeq>;

    template <typename Table, std::size_t size>
    constexpr auto make_string(char const (&str)[size]) {
        using pair = typename Table::template index<MAKE_RAND_VAL(Table::size)>;
        constexpr Encoder encoder = pair::template index<0>::value;
        constexpr Decoder decoder = pair::template index<1>::value;

        return make_string<encoder, decoder>(str);
    }
}


#endif
