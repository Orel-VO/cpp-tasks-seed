#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "base85ed.h"

std::vector<uint8_t> read_stdin_to_vector_iostream()
{
    constexpr std::streamsize BUF_SIZE = 64 * 1024;
    std::vector<uint8_t> out;
    out.reserve(1024);
    std::vector<char> buf(BUF_SIZE);

    std::ios::sync_with_stdio(false);
    while (std::cin)
    {
        std::cin.read(buf.data(), BUF_SIZE);
        const std::streamsize n = std::cin.gcount();
        if (n > 0)
        {
            out.insert(out.end(), reinterpret_cast<uint8_t *>(buf.data()),
                       reinterpret_cast<uint8_t *>(buf.data() + n));
        }
        if (n < BUF_SIZE)
        {
            break;
        }
    }
    return out;
}

void write_vector_to_stdout(const std::vector<uint8_t> &data)
{
    std::ios::sync_with_stdio(false);
    if (!data.empty())
    {
        std::cout.write(reinterpret_cast<const char *>(data.data()), data.size());
    }
    std::cout.flush();
}

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Use -e or -d argument\n";
        return 1;
    }

    std::function<std::vector<uint8_t>(const std::vector<uint8_t> &)> func;
    const std::string arg = argv[1];
    if (arg == "--encode" || arg == "-e")
    {
        func = base85::encode;
    }
    else if (arg == "--decode" || arg == "-d")
    {
        func = base85::decode;
    }
    else
    {
        std::cerr << "Don't know how to deal with <" << arg << ">, use -e or -d\n";
        return 1;
    }

    const auto data = read_stdin_to_vector_iostream();
    const auto result = func(data);
    write_vector_to_stdout(result);
    return 0;
}
