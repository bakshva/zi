#pragma once
#include <cstdint>
#include <vector>
#include <array>
#include <openssl/md5.h>

inline std::array<uint8_t, 16> md5(const std::vector<char>& data) {
    std::array<uint8_t, 16> digest{};
    MD5(reinterpret_cast<const unsigned char*>(data.data()), data.size(), digest.data());
    return digest;
}