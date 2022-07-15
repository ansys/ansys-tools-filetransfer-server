#pragma once

#include <string>

#ifdef _MSC_VER
#pragma warning(push, 3)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <boost/filesystem/path.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif

namespace file_transfer {
namespace detail {
std::string get_sha1_hex_digest(
    const boost::filesystem::path& path_, const std::size_t chunk_size_ = 1024);
} // namespace detail
} // namespace file_transfer
