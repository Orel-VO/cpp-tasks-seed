#include <gtest/gtest.h>
#include <vector>
#include <cstdint>
#include <string>
#include <random>
#include <algorithm>

#include "base85ed.h"

// ============================================================================
// Вспомогательные функции
// ============================================================================

static std::vector<uint8_t> cstr2v(const char *s)
{
    return std::vector<uint8_t>(
        reinterpret_cast<const uint8_t*>(s),
        reinterpret_cast<const uint8_t*>(s) + std::string(s).size()
    );
}

static std::vector<uint8_t> string2v(const std::string& s)
{
    return std::vector<uint8_t>(s.begin(), s.end());
}

static std::string v2string(const std::vector<uint8_t>& v)
{
    return std::string(v.begin(), v.end());
}

// Генерация случайных данных для тестов
static std::vector<uint8_t> generate_random_bytes(size_t size, uint32_t seed = 42)
{
    std::mt19937 rng(seed);
    std::uniform_int_distribution<uint8_t> dist(0, 255);
    
    std::vector<uint8_t> result;
    result.reserve(size);
    for (size_t i = 0; i < size; ++i)
    {
        result.push_back(dist(rng));
    }
    return result;
}

// ============================================================================
// Базовые тесты из условия
// ============================================================================

TEST(Base85Encode, EmptyString)
{
    EXPECT_EQ(base85::encode(cstr2v("")), cstr2v(""));
}

TEST(Base85Encode, SingleByte)
{
    // '1' в ASCII = 49 (0x31)
    // Ожидаемый результат 'F#' для Adobe Ascii85
    auto encoded = base85::encode(cstr2v("1"));
    EXPECT_EQ(v2string(encoded), "F#~>");
}

TEST(Base85Encode, TwoBytes)
{
    auto encoded = base85::encode(cstr2v("12"));
    EXPECT_EQ(v2string(encoded), "F){~>");
}

TEST(Base85Encode, ThreeBytes)
{
    auto encoded = base85::encode(cstr2v("123"));
    EXPECT_EQ(v2string(encoded), "F)}j~>");
}

TEST(Base85Encode, FourBytes)
{
    auto encoded = base85::encode(cstr2v("1234"));
    EXPECT_EQ(v2string(encoded), "F)}kW~>");
}

// ============================================================================
// Тесты декодирования
// ============================================================================

TEST(Base85Decode, EmptyString)
{
    EXPECT_EQ(base85::decode(cstr2v("")), cstr2v(""));
}

TEST(Base85Decode, SingleByte)
{
    auto decoded = base85::decode(cstr2v("F#~>"));
    EXPECT_EQ(v2string(decoded), "1");
}

TEST(Base85Decode, TwoBytes)
{
    auto decoded = base85::decode(cstr2v("F){~>"));
    EXPECT_EQ(v2string(decoded), "12");
}

TEST(Base85Decode, ThreeBytes)
{
    auto decoded = base85::decode(cstr2v("F)}j~>"));
    EXPECT_EQ(v2string(decoded), "123");
}

TEST(Base85Decode, FourBytes)
{
    auto decoded = base85::decode(cstr2v("F)}kW~>"));
    EXPECT_EQ(v2string(decoded), "1234");
}

// ============================================================================
// Тесты с нулевыми байтами (специальный символ 'z')
// ============================================================================

TEST(Base85Encode, ZeroBytes)
{
    // 4 нулевых байта должны кодироваться как 'z'
    std::vector<uint8_t> zeros(4, 0);
    auto encoded = base85::encode(zeros);
    EXPECT_EQ(v2string(encoded), "z~>");
}

TEST(Base85Encode, EightZeroBytes)
{
    // 8 нулевых байтов -> 'zz'
    std::vector<uint8_t> zeros(8, 0);
    auto encoded = base85::encode(zeros);
    EXPECT_EQ(v2string(encoded), "zz~>");
}

TEST(Base85Decode, ZeroByteSpecial)
{
    auto decoded = base85::decode(cstr2v("z~>"));
    std::vector<uint8_t> expected(4, 0);
    EXPECT_EQ(decoded, expected);
}

TEST(Base85Decode, TwoZeroBlocks)
{
    auto decoded = base85::decode(cstr2v("zz~>"));
    std::vector<uint8_t> expected(8, 0);
    EXPECT_EQ(decoded, expected);
}

// ============================================================================
// Тесты смешанных данных (нулевые и ненулевые)
// ============================================================================

TEST(Base85Encode, MixedZeroAndNonZero)
{
    // "123\0\0\0\0456" (4 нулевых байта в середине)
    std::vector<uint8_t> data = cstr2v("123");
    data.insert(data.end(), 4, 0);
    auto rest = cstr2v("456");
    data.insert(data.end(), rest.begin(), rest.end());
    
    auto encoded = base85::encode(data);
    // Первые 3 байта "123" -> 4 символа, затем 'z', затем "456" -> 4 символа
    // Всего ожидается: [4 символа] + z + [4 символа] + ~>
    EXPECT_GT(encoded.size(), 0);
    
    // Проверяем через декодирование
    auto decoded = base85::decode(encoded);
    EXPECT_EQ(decoded, data);
}

// ============================================================================
// Циклические тесты (encode(decode(x)) == x)
// ============================================================================

TEST(Base85Roundtrip, ShortStrings)
{
    std::vector<std::string> test_strings = {
        "",
        "a",
        "ab",
        "abc",
        "abcd",
        "Hello",
        "World!",
        "Base85 test",
        "Special chars: \x00\x01\x02\x03\xFF",
        "Longer string with multiple blocks of data for testing purposes"
    };
    
    for (const auto& original_str : test_strings)
    {
        auto original = string2v(original_str);
        auto encoded = base85::encode(original);
        auto decoded = base85::decode(encoded);
        EXPECT_EQ(decoded, original) << "Failed for: " << original_str;
    }
}

TEST(Base85Roundtrip, RandomData)
{
    std::vector<size_t> sizes = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 
                                  15, 16, 17, 31, 32, 33, 63, 64, 65,
                                  127, 128, 129, 255, 256, 257, 511, 512, 1024};
    
    for (size_t size : sizes)
    {
        for (uint32_t seed = 1; seed <= 5; ++seed)
        {
            auto original = generate_random_bytes(size, seed);
            auto encoded = base85::encode(original);
            auto decoded = base85::decode(encoded);
            EXPECT_EQ(decoded, original) 
                << "Roundtrip failed for size=" << size << ", seed=" << seed;
        }
    }
}

// ============================================================================
// Тесты граничных значений
// ============================================================================

TEST(Base85Boundary, MaximumByteValues)
{
    std::vector<uint8_t> data = {255, 255, 255, 255};
    auto encoded = base85::encode(data);
    auto decoded = base85::decode(encoded);
    EXPECT_EQ(decoded, data);
}

TEST(Base85Boundary, AlternatingBytes)
{
    std::vector<uint8_t> data;
    for (int i = 0; i < 256; ++i)
    {
        data.push_back(static_cast<uint8_t>(i));
    }
    auto encoded = base85::encode(data);
    auto decoded = base85::decode(encoded);
    EXPECT_EQ(decoded, data);
}

TEST(Base85Boundary, AllPossibleByteValues)
{
    std::vector<uint8_t> data;
    for (int i = 0; i < 256; ++i)
    {
        data.push_back(static_cast<uint8_t>(i));
        data.push_back(static_cast<uint8_t>(i));
        data.push_back(static_cast<uint8_t>(i));
        data.push_back(static_cast<uint8_t>(i));
    }
    auto encoded = base85::encode(data);
    auto decoded = base85::decode(encoded);
    EXPECT_EQ(decoded, data);
}

// ============================================================================
// Тесты больших данных
// ============================================================================

TEST(Base85Large, TenKilobytes)
{
    auto data = generate_random_bytes(10000, 12345);
    auto encoded = base85::encode(data);
    auto decoded = base85::decode(encoded);
    EXPECT_EQ(decoded, data);
    EXPECT_EQ(encoded.size(), (data.size() + 3) / 4 * 5 + 2);
}

TEST(Base85Large, HundredKilobytes)
{
    auto data = generate_random_bytes(100000, 99999);
    auto encoded = base85::encode(data);
    auto decoded = base85::decode(encoded);
    EXPECT_EQ(decoded, data);
}

// ============================================================================
// Тесты на выброс исключений (некорректные входные данные)
// ============================================================================

TEST(Base85DecodeInvalid, MissingSuffix)
{
    EXPECT_THROW(base85::decode(cstr2v("F#")), std::invalid_argument);
    EXPECT_THROW(base85::decode(cstr2v("F){")), std::invalid_argument);
}

TEST(Base85DecodeInvalid, InvalidCharacters)
{
    // Символ за пределами диапазона !..u
    EXPECT_THROW(base85::decode(cstr2v("F#v~>")), std::invalid_argument);
    EXPECT_THROW(base85::decode(cstr2v("F# ~>")), std::invalid_argument);
    EXPECT_THROW(base85::decode(cstr2v("F#\x00~>")), std::invalid_argument);
}

TEST(Base85DecodeInvalid, IncompleteBlock)
{
    // Слишком короткий последний блок
    EXPECT_THROW(base85::decode(cstr2v("F~>")), std::invalid_argument);
    EXPECT_THROW(base85::decode(cstr2v("zF~>")), std::invalid_argument);
}

TEST(Base85DecodeInvalid, ZInFiveCharBlock)
{
    // 'z' внутри 5-символьного блока недопустим
    EXPECT_THROW(base85::decode(cstr2v("abcdz~>")), std::invalid_argument);
    EXPECT_THROW(base85::decode(cstr2v("azbcd~>")), std::invalid_argument);
}

// ============================================================================
// Тесты свойств кодирования
// ============================================================================

TEST(Base85Properties, EncodedLength)
{
    auto data = generate_random_bytes(100, 1);
    auto encoded = base85::encode(data);
    
    // Длина закодированных данных с суффиксом ~>
    // Каждые 4 байта -> 5 символов + возможно неполный последний блок
    size_t expected_blocks = (data.size() + 3) / 4;
    size_t expected_len = 0;
    
    for (size_t i = 0; i < expected_blocks - 1; ++i)
    {
        expected_len += 5; // полные блоки
    }
    
    // Последний блок
    size_t remaining = data.size() % 4;
    if (remaining == 0 && data.size() > 0)
    {
        expected_len += 5; // полный блок
    }
    else if (remaining == 1)
    {
        expected_len += 2;
    }
    else if (remaining == 2)
    {
        expected_len += 3;
    }
    else if (remaining == 3)
    {
        expected_len += 4;
    }
    
    if (data.size() > 0)
    {
        expected_len += 2; // суффикс ~>
    }
    
    EXPECT_EQ(encoded.size(), expected_len);
}

TEST(Base85Properties, AsciiRange)
{
    auto data = generate_random_bytes(1000, 42);
    auto encoded = base85::encode(data);
    
    // Все символы должны быть в диапазоне 33-117 или быть 'z' или '~' или '>'
    for (uint8_t c : encoded)
    {
        if (c != 'z' && c != '~' && c != '>')
        {
            EXPECT_GE(c, 33);
            EXPECT_LE(c, 117);
        }
    }
}

// ============================================================================
// Тесты конкретных примеров из спецификации
// ============================================================================

TEST(Base85Examples, AdobeExample)
{
    // Пример из Adobe: "Man" -> "9jqo^"
    // Согласно спецификации Ascii85
    auto encoded = base85::encode(cstr2v("Man"));
    // Ожидаем "9jqo^~>" (с суффиксом)
    auto decoded = base85::decode(encoded);
    EXPECT_EQ(v2string(decoded), "Man");
}

TEST(Base85Examples, WikipediaExample)
{
    // Пример из Wikipedia: "easy" -> "<~ARTY*~>"
    auto encoded = base85::encode(cstr2v("easy"));
    auto decoded = base85::decode(encoded);
    EXPECT_EQ(v2string(decoded), "easy");
}

// ============================================================================
// Тесты консистентности
// ============================================================================

TEST(Base85Consistency, MultipleEncodeDecode)
{
    auto original = generate_random_bytes(256, 777);
    auto encoded1 = base85::encode(original);
    auto encoded2 = base85::encode(original);
    
    // Кодирование должно быть детерминированным
    EXPECT_EQ(encoded1, encoded2);
    
    auto decoded = base85::decode(encoded1);
    EXPECT_EQ(decoded, original);
}

TEST(Base85Consistency, Idempotence)
{
    // decode(encode(x)) должно давать x
    auto original = generate_random_bytes(128, 888);
    auto encoded = base85::encode(original);
    auto decoded = base85::decode(encoded);
    EXPECT_EQ(decoded, original);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
