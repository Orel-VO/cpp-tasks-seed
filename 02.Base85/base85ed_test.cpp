#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "base85ed.h"

namespace
{

const std::vector<std::pair<const char *, const char *>> short_cases =
{
    {"", ""},
    {"F#", "1"},
    {"F){", "12"},
    {"F)}j", "123"},
    {"F)}kW", "1234"},
};

std::vector<uint8_t> cstr2v(const char *s)
{
    return std::vector<uint8_t>(s, s + std::string(s).size());
}

std::vector<uint8_t> make_bytes(size_t n)
{
    std::vector<uint8_t> data(n);
    for (size_t i = 0; i < n; ++i)
    {
        data[i] = static_cast<uint8_t>((i * 131 + 17) % 256);
    }
    return data;
}

}

TEST(Base85ShortsEncode, TrivialShortEncodes)
{
    for (const auto &p : short_cases)
    {
        EXPECT_EQ(base85::encode(cstr2v(p.second)), cstr2v(p.first));
    }
}

TEST(Base85ShortsDecode, TrivialShortDecodes)
{
    for (const auto &p : short_cases)
    {
        EXPECT_EQ(base85::decode(cstr2v(p.first)), cstr2v(p.second));
    }
}

TEST(Base85Roundtrip, TextDifferentLengths)
{
    const std::string seed =
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
        "Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";

    for (size_t len = 0; len <= 300; ++len)
    {
        const std::string text = seed.substr(0, std::min(len, seed.size())) +
                                 std::string(len > seed.size() ? len - seed.size() : 0, 'x');
        const auto input = std::vector<uint8_t>(text.begin(), text.end());
        const auto encoded = base85::encode(input);
        const auto decoded = base85::decode(encoded);
        EXPECT_EQ(decoded, input) << "length=" << len;
    }
}

TEST(Base85Roundtrip, BinaryDifferentLengths)
{
    for (size_t len = 0; len <= 1024; ++len)
    {
        const auto input = make_bytes(len);
        const auto encoded = base85::encode(input);
        const auto decoded = base85::decode(encoded);
        EXPECT_EQ(decoded, input) << "length=" << len;
    }
}

TEST(Base85Decode, InvalidTailLengthThrows)
{
    EXPECT_THROW(base85::decode(cstr2v("Z")), std::invalid_argument);
}

TEST(Base85Decode, InvalidCharacterThrows)
{
    EXPECT_THROW(base85::decode(cstr2v("~~~~~[")), std::invalid_argument);
}
