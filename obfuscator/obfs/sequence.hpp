#ifndef COMPILE_TIME_SEQUENCE
#define COMPILE_TIME_SEQUENCE

#include <type_traits>

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

#endif