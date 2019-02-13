#ifndef STRING_OBFS
#define STRING_OBFS

#include <utility>

template <size_t size, typename D>
class StringObfs {
public:
    template <typename E, typename Ds, size_t... Idx>
    constexpr StringObfs(char const* str,
                          E&& encoder,
                          Ds&& decoder,
                         std::index_sequence<Idx...>):
        str{ encoder(str[Idx])... },
        decoder(std::forward<Ds>(decoder)) {
        // Do Nothing
    }

    constexpr size_t len() const {
        return size;
    }

    inline const char* decode() const {
        for (size_t i = 0; i < size; ++i) {
            str[i] = decoder(str[i]);
        }
        return str;
    }

private:
    mutable char str[size];
    D decoder;
};

template <size_t len, typename E, typename D>
constexpr auto make_stringobfs(char const (&str)[len],
                               E&& encoder,
                               D&& decoder) {
    return StringObfs<len, D>(str,
                              std::forward<E>(encoder),
                              std::forward<D>(decoder), 
                              std::make_index_sequence<len>());
}

#endif