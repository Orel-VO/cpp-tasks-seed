#include "base85ed.h"

#include <algorithm>
#include <array>
#include <stdexcept>

namespace
{

constexpr char kAlphabet[] =
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!#$%&()*+-;<=>?@^_`{|}~";

std::array<int, 256> build_reverse_table()
{
    std::array<int, 256> table{};
    table.fill(-1);
    for (int i = 0; i < 85; ++i)
    {
        table[static_cast<unsigned char>(kAlphabet[i])] = i;
    }
    return table;
}

const std::array<int, 256> kReverseTable = build_reverse_table();

}

std::vector<uint8_t> base85::encode(const std::vector<uint8_t> &bytes)
{
    std::vector<uint8_t> out;
    out.reserve((bytes.size() / 4) * 5 + (bytes.size() % 4 == 0 ? 0 : bytes.size() % 4 + 1));

    size_t pos = 0;
    while (pos + 4 <= bytes.size())
    {
        uint32_t value = (static_cast<uint32_t>(bytes[pos]) << 24) |
                         (static_cast<uint32_t>(bytes[pos + 1]) << 16) |
                         (static_cast<uint32_t>(bytes[pos + 2]) << 8) |
                         static_cast<uint32_t>(bytes[pos + 3]);

        std::array<uint8_t, 5> block{};
        for (int i = 4; i >= 0; --i)
        {
            block[i] = static_cast<uint8_t>(kAlphabet[value % 85]);
            value /= 85;
        }
        out.insert(out.end(), block.begin(), block.end());
        pos += 4;
    }

    const size_t rem = bytes.size() - pos;
    if (rem > 0)
    {
        uint32_t value = 0;
        for (size_t i = 0; i < rem; ++i)
        {
            value |= static_cast<uint32_t>(bytes[pos + i]) << (24 - 8 * i);
        }

        std::array<uint8_t, 5> block{};
        for (int i = 4; i >= 0; --i)
        {
            block[i] = static_cast<uint8_t>(kAlphabet[value % 85]);
            value /= 85;
        }
        out.insert(out.end(), block.begin(), block.begin() + rem + 1);
    }

    return out;
}

std::vector<uint8_t> base85::decode(const std::vector<uint8_t> &b85str)
{
    std::vector<uint8_t> out;
    out.reserve((b85str.size() / 5) * 4 + (b85str.size() % 5 == 0 ? 0 : b85str.size() % 5 - 1));

    size_t pos = 0;
    while (pos < b85str.size())
    {
        const size_t chunk_size = std::min(static_cast<size_t>(5), b85str.size() - pos);
        if (chunk_size == 1)
        {
            throw std::invalid_argument("base85: invalid tail length");
        }

        uint32_t value = 0;
        for (size_t i = 0; i < chunk_size; ++i)
        {
            const int digit = kReverseTable[b85str[pos + i]];
            if (digit < 0)
            {
                throw std::invalid_argument("base85: invalid character");
            }
            value = value * 85 + static_cast<uint32_t>(digit);
        }
        for (size_t i = chunk_size; i < 5; ++i)
        {
            value = value * 85 + 84;
        }

        uint8_t block[4] = {};
        block[0] = static_cast<uint8_t>((value >> 24) & 0xFF);
        block[1] = static_cast<uint8_t>((value >> 16) & 0xFF);
        block[2] = static_cast<uint8_t>((value >> 8) & 0xFF);
        block[3] = static_cast<uint8_t>(value & 0xFF);

        const size_t out_count = (chunk_size == 5) ? 4 : chunk_size - 1;
        out.insert(out.end(), block, block + out_count);
        pos += chunk_size;
    }

    return out;
}
