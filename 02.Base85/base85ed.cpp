#include <vector>
#include <cstdint>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <iostream>

#include "base85ed.h"

namespace base85
{

static constexpr uint32_t BASE = 85;
static constexpr uint8_t FIRST_CHAR = '!';
static constexpr uint8_t LAST_CHAR = 'u';

static void encode_block(const uint8_t* input, uint8_t* output)
{
    uint32_t value = 0;
    value |= static_cast<uint32_t>(input[0]) << 24;
    value |= static_cast<uint32_t>(input[1]) << 16;
    value |= static_cast<uint32_t>(input[2]) << 8;
    value |= static_cast<uint32_t>(input[3]);

    if (value == 0)
    {
        output[0] = 'z';
        return;
    }

    // Правильный порядок: сначала старший разряд (output[0])
    uint32_t temp = value;
    for (int i = 4; i >= 0; --i)
    {
        output[i] = static_cast<uint8_t>((temp % BASE) + FIRST_CHAR);
        temp /= BASE;
    }
}

static void decode_block(const uint8_t* input, uint8_t* output)
{
    if (input[0] == 'z')
    {
        for (int i = 0; i < 4; ++i)
        {
            output[i] = 0;
        }
        return;
    }

    uint32_t value = 0;
    for (int i = 0; i < 5; ++i)
    {
        if (input[i] < FIRST_CHAR || input[i] > LAST_CHAR)
        {
            throw std::invalid_argument("Invalid Base85 character");
        }
        value = value * BASE + static_cast<uint32_t>(input[i] - FIRST_CHAR);
    }

    output[0] = static_cast<uint8_t>((value >> 24) & 0xFF);
    output[1] = static_cast<uint8_t>((value >> 16) & 0xFF);
    output[2] = static_cast<uint8_t>((value >> 8) & 0xFF);
    output[3] = static_cast<uint8_t>(value & 0xFF);
}

std::vector<uint8_t> encode(std::vector<uint8_t> const &bytes)
{
    std::vector<uint8_t> result;
    size_t n = bytes.size();

    if (n == 0)
    {
        return result;
    }

    size_t i = 0;

    while (i + 4 <= n)
    {
        if (bytes[i] == 0 && bytes[i+1] == 0 && bytes[i+2] == 0 && bytes[i+3] == 0)
        {
            result.push_back('z');
        }
        else
        {
            uint8_t encoded[5];
            encode_block(&bytes[i], encoded);
            for (int j = 0; j < 5; ++j)
            {
                result.push_back(encoded[j]);
            }
        }
        i += 4;
    }

    size_t remaining = n - i;
    if (remaining > 0)
    {
        uint8_t block[4] = {0, 0, 0, 0};
        for (size_t j = 0; j < remaining; ++j)
        {
            block[j] = bytes[i + j];
        }

        uint8_t encoded[5];
        encode_block(block, encoded);

        // Для неполного блока берем (remaining + 1) символов
        for (size_t j = 0; j < remaining + 1; ++j)
        {
            result.push_back(encoded[j]);
        }
    }

    result.push_back('~');
    result.push_back('>');

    return result;
}

std::vector<uint8_t> decode(std::vector<uint8_t> const &b85str)
{
    if (b85str.empty())
    {
        return std::vector<uint8_t>();
    }

    if (b85str.size() < 2)
    {
        throw std::invalid_argument("Invalid Base85 string: too short");
    }

    size_t len = b85str.size();
    if (b85str[len - 2] != '~' || b85str[len - 1] != '>')
    {
        throw std::invalid_argument("Invalid Base85 string: missing '~>' suffix");
    }

    std::vector<uint8_t> result;
    size_t i = 0;
    size_t n = len - 2;

    while (i < n)
    {
        if (b85str[i] == 'z')
        {
            result.push_back(0);
            result.push_back(0);
            result.push_back(0);
            result.push_back(0);
            ++i;
            continue;
        }

        if (i + 5 <= n)
        {
            uint8_t block[5];
            for (int j = 0; j < 5; ++j)
            {
                block[j] = b85str[i + j];
                if (block[j] == 'z')
                {
                    throw std::invalid_argument("Invalid Base85: 'z' inside 5-char block");
                }
                if (block[j] < FIRST_CHAR || block[j] > LAST_CHAR)
                {
                    throw std::invalid_argument("Invalid Base85 character");
                }
            }

            uint8_t decoded[4];
            decode_block(block, decoded);
            for (int j = 0; j < 4; ++j)
            {
                result.push_back(decoded[j]);
            }
            i += 5;
        }
        else
        {
            size_t remaining = n - i;
            if (remaining < 2)
            {
                throw std::invalid_argument("Invalid Base85: incomplete block");
            }

            uint8_t block[5] = {FIRST_CHAR, FIRST_CHAR, FIRST_CHAR, FIRST_CHAR, FIRST_CHAR};
            for (size_t j = 0; j < remaining; ++j)
            {
                block[j] = b85str[i + j];
                if (block[j] == 'z')
                {
                    throw std::invalid_argument("Invalid Base85: 'z' inside 5-char block");
                }
                if (block[j] < FIRST_CHAR || block[j] > LAST_CHAR)
                {
                    throw std::invalid_argument("Invalid Base85 character");
                }
            }

            uint8_t decoded[4];
            decode_block(block, decoded);

            for (size_t j = 0; j < remaining - 1; ++j)
            {
                result.push_back(decoded[j]);
            }
            break;
        }
    }

    return result;
}

} // namespace base85
