#ifndef COMPILE_TIME_SEQUENCE
#define COMPILE_TIME_SEQUENCE

#include <type_traits>

namespace obfs {
    template <typename T, T Val, T... Others>
    struct Sequence {
        using value_type = T;
        constexpr static T value = Val;

        using next = Sequence<T, Others...>;

        template <std::size_t Idx>
        constexpr static T get() {
            if constexpr (Idx == 0) {
                return value;
            }
            else {
                return next::get<Idx - 1>();
            }
        }
    };

    template <typename T, T Val>
    struct Sequence<T, Val> {
        using value_type = T;
        constexpr static T value = Val;

        template <std::size_t Idx>
        constexpr static T get() {
            static_assert(Idx == 0, "Couldn't get proper value from sequence");
            return value;
        }
    };

    template <typename T, typename... Ts>
    struct TypeSeq {
        using type = T;
        using next = TypeSeq<Ts...>;

        // template <std::size_t Idx>
        // using get = std::conditional_t<Idx == 0, type, next::get<Idx - 1>>;
    };

    template <typename T>
    struct TypeSeq<T> {
        using type = T;

        // template <std::size_t Idx>
        // using get = std::enable_if_t<Idx == 0, type>;
    };

    template <typename... T>
    struct SeqPack {
        constexpr static std::size_t size = sizeof...(T);

        template <std::size_t Idx>
        using get = TypeSeq<Sequence<typename T::value_type, T::get<Idx>>...>;
    };
}

#endif