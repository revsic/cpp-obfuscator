#ifndef OBFS_STRING
#define OBFS_STRING

#include "random.hpp"
#include "sequence.hpp"

#include <utility>

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

    template <Encoder encoder, Decoder decoder>
    using encoder_pair = Sequence<Encoder, encoder, decoder>;

    template <typename... Seq>
    using make_pair_table = TypeSeq<Seq...>;

    template <typename Table, std::size_t size>
    constexpr auto make_string(char const(&str)[size]) {
        using pair = typename Table::template index<MAKE_RAND_VAL(Table::size)>;
        constexpr Encoder encoder = pair::template index<0>::value;
        constexpr Decoder decoder = pair::template index<1>::value;

        return make_string<encoder, decoder>(str);
    }
}

#define MAKE_STRING(Var, String, ...) constexpr auto Var = obfs::make_string<__VA_ARGS__>(String);

#endif