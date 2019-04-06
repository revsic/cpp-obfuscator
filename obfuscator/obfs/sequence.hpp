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

        constexpr static std::size_t size = 1 + sizeof...(Ts);

        template <std::size_t Idx>
        using index = std::conditional_t<Idx == 0, type, typename next::template index<Idx - 1>>;
    };

    template <typename T>
    struct TypeSeq<T> {
        using type = T;

        constexpr static std::size_t size = 1;

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
        constexpr static std::size_t size = MinVal<T::size...>::value;

        template <std::size_t Idx, typename U>
        using getter = typename U::template index<Idx>;

        template <std::size_t Idx>
        using index = TypeSeq<getter<Idx, T>...>;
    };

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
}

#endif