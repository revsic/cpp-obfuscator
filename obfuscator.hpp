#ifndef OBFUSCATOR_HPP
#define OBFUSCATOR_HPP

#include <utility>

#define STRING_OBFS


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
            for (char& chr : str) {
                chr = decoder(chr);
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
