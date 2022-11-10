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
auto get_sha1_hex_digest(
    const boost::filesystem::path& path_,
    const std::streamsize chunk_size_ = 1024
) -> std::string;
} // namespace detail
} // namespace file_transfer
