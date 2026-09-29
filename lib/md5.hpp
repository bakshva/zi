#ifndef MD5_HPP
#define MD5_HPP

#include <array>
#include <cstdint>
#include <vector>

std::array<uint8_t, 16> md5(const std::vector<char> &data);

#endif