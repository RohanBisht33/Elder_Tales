#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace Core {
namespace Crypto {

    // Calculate SHA-256 Hash
    std::string SHA256(const std::string& input);
    std::string SHA256(const std::vector<uint8_t>& input);

    // Calculate HMAC-SHA256
    std::string HMAC_SHA256(const std::string& key, const std::vector<uint8_t>& data);

} // namespace Crypto
} // namespace Core
