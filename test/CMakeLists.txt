find_package(GTest REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)

include(GoogleTest)

add_library(
    test_utils
    STATIC
    test_utils.cpp
)
target_link_libraries(
    test_utils
    Boost::filesystem
)


list(APPEND TestNames "test_sha")

foreach(test_name IN LISTS TestNames)
    add_executable(${test_name} ${test_name}.cpp)

    target_link_libraries(
        ${test_name}
        test_utils
        filetransfer_service
        GTest::gtest_main
    )

    gtest_discover_tests(
        ${test_name}
        PROPERTIES ENVIRONMENT "TEST_DATA_DIR=${CMAKE_CURRENT_SOURCE_DIR}/data"
    )
endforeach()
