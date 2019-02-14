#ifndef STRING_OBFS
#define STRING_OBFS

#include <utility>

namespace obfs {
    template <size_t size, char(*encoder)(char), char(*decoder)(char)>
    class String {
    public:
        template <size_t... Idx>
        constexpr String(char const* str,
                         std::index_sequence<Idx...>):
            str{ encoder(str[Idx])... } {
            // Do Nothing
        }

        constexpr size_t len() const {
            return size;
        }

        inline char const* decode() const {
            for (size_t i = 0; i < size; ++i) {
                str[i] = decoder(str[i]);
            }
            return str;
        }

    private:
        mutable char str[size];
    };

    template <char(*encoder)(char), char(*decoder)(char), size_t size>
    constexpr auto make_string(char const (&str)[size]) {
        return String<size, encoder, decoder>(str, std::make_index_sequence<size>());
    }
}

#endif