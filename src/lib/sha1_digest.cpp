
#include "sha1_digest.h"

#include <iomanip>
#include <ios>
#include <ranges>
#include <sstream>
#include <string>

#ifdef _MSC_VER
#pragma warning(push, 3)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/uuid/detail/sha1.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif

namespace file_transfer::detail {
auto get_sha1_hex_digest(
    const boost::filesystem::path& path_, const std::streamsize chunk_size_
) -> std::string {
    std::string buffer(chunk_size_, '\0');
    boost::filesystem::ifstream in_file{path_, std::ios_base::binary};
    boost::uuids::detail::sha1 sha_value{};
    if (!in_file.good()) {
        throw std::runtime_error("Could not open file.");
    }
    while (in_file.good()) {
        in_file.read(buffer.data(), chunk_size_);
        sha_value.process_bytes(buffer.data(), in_file.gcount());
    }
    boost::uuids::detail::sha1::digest_type res_int;
    sha_value.get_digest(res_int);

    // std::format not yet supported in our toolchains
    std::stringstream res_stream;
    res_stream << std::hex;
    for (const auto& elem : res_int) {
        res_stream << std::setfill('0') << std::setw(8) << elem;
    }
    return res_stream.str();
}
} // namespace file_transfer::detail
