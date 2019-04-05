#ifndef OBFUSCATOR_HPP
#define OBFUSCATOR_HPP

#include <type_traits>
#include <utility>

#define OBFS_FINITE_STATE_MACHINE
#define COMPILE_TIME_RANDOM
#define MAKE_RAND_VAL(MOD) RAND_VAL<__LINE__, MOD>
#define COMPILE_TIME_SEQUENCE
#define OBFS_STRING
#define MAKE_STRING(Var, String, ...) constexpr auto Var = obfs::make_string<__VA_ARGS__>(String);


namespace obfs {
    struct Pass {};

    template <typename IfAllPass, typename T, typename... Ts>
    struct First {
        using type = std::conditional_t<
            std::is_same_v<T, Pass>,
            typename First<IfAllPass, Ts...>::type,
            T>;
    };

    template <typename IfAllPass, typename T>
    struct First<IfAllPass, T> {
        using type = std::conditional_t<
            std::is_same_v<T, Pass>,
            IfAllPass,
            T>;
    };

    constexpr void FreeAction() {}

    template <typename Event, typename State, void(*Action)() = FreeAction>
    struct Next {
        using event = Event;
        using state = State;
        constexpr static void(*action)() = Action;
    };

    struct None {};

    template <typename State, typename... Nexts>
    struct Stage {
        using state = State;

        template <typename NextInfo, typename Event>
        using act = std::conditional_t<
            std::is_same_v<typename NextInfo::event, Event>, NextInfo, Pass>;

        template <typename Event>
        using next = typename First<None, act<Nexts, Event>...>::type;
    };

    template <typename... Specs>
    struct StateMachine {
        template <typename State, typename StageT>
        using filter = std::conditional_t<
            std::is_same_v<typename StageT::state, State>, StageT, Pass>;

        template <typename State>
        using find = typename First<None, filter<State, Specs>...>::type;

        template <typename Stage_, typename Event>
        struct next {
            using type = typename Stage_::template next<Event>;
        };

        template <typename Event>
        struct next<None, Event> {
            using type = None;
        };

        template <typename State, typename Event>
        using next_t = typename next<find<State>, Event>::type;

        template <typename State>
        struct invoker {
            static auto action() {
                State::action();
                return State{};
            }
        };

        template <>
        struct invoker<None> {
            static auto action() {
                return None{};
            }
        };

        template <typename State, typename Event>
        static auto run(State state, Event event) {
            using next_state = next_t<State, Event>;
            return invoker<next_state>::action();
        }
    };
}


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

        constexpr static std::size_t size = 1 + sizeof...(Others);

        template <std::size_t Idx>
        using index = std::conditional_t<Idx == 0, value, typename next::template index<Idx - 1>>;
    };

    template <typename T, T Val>
    struct Sequence<T, Val> {
        using value = TypeVal<T, Val>;

        constexpr static std::size_t size = 1;

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

    template <std::size_t Val, std::size_t... Others>
    struct MinVal {
        constexpr static std::size_t value = 
            Val < MinVal<Others...>::value ? Val : MinVal<Others...>::value;
    };

    template <std::size_t Val>
    struct MinVal<Val> {
        constexpr static std::size_t value = Val;
    };

    template <typename... T>
    struct SeqPack {
        constexpr static std::size_t size = sizeof...(T);
        constexpr static std::size_t elem_size = MinVal<T::size...>::value;

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
        using pair = typename Table::template index<MAKE_RAND_VAL(Table::elem_size)>;
        constexpr Encoder encoder = pair::template index<0>::value;
        constexpr Decoder decoder = pair::template index<1>::value;

        return make_string<encoder, decoder>(str);
    }
}


#endif
