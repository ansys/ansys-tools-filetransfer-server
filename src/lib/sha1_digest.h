#pragma once

#include <filesystem>
#include <string>

namespace file_transfer {
namespace detail {
std::string get_sha1_hex_digest(
    const std::filesystem::path& path_, const std::size_t chunk_size_ = 1024);
} // namespace detail
} // namespace file_transfer
