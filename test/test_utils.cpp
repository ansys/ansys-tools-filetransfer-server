#include "test_utils.h"

namespace test_utils {

boost::filesystem::path get_test_data_dir() {
    return std::getenv("TEST_DATA_DIR");
}

} // namespace test_utils
