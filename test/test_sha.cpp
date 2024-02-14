#include <gtest/gtest.h>

#include "sha1_digest.h"

#include "test_utils.h"

namespace {

TEST(sha, emptyfile) {
    // Test the SHA1 hex digest of an empty file.
    const auto empty_file = test_utils::get_test_data_dir() / "empty-file";
    const auto sha1_digest_res =
        file_transfer::detail::get_sha1_hex_digest(empty_file);
    EXPECT_EQ(sha1_digest_res, "da39a3ee5e6b4b0d3255bfef95601890afd80709");
}

TEST(sha, nonemptyfile) {
    // Test the SHA1 hex digest of a non-empty file.
    const auto non_empty_file =
        test_utils::get_test_data_dir() / "non-empty-file";
    const auto sha1_digest_res =
        file_transfer::detail::get_sha1_hex_digest(non_empty_file);
    EXPECT_EQ(sha1_digest_res, "2817cb94c81232aa658716f369baf775c9707b11");
}

} // namespace
