#ifndef __BASE85ED__
#define __BASE85ED__

#include <vector>
#include <cstdint>
#include <stdexcept>


namespace base85
{


class Base85Exception : public std::runtime_error {
public:
    explicit Base85Exception(const std::string& msg) : std::runtime_error(msg) {}
};


std::vector<uint8_t> encode(std::vector<uint8_t> const &bytes);


std::vector<uint8_t> decode(std::vector<uint8_t> const &b85str);


#endif 
