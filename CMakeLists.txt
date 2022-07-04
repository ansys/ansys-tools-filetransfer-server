project(ansys_utilities_filetransfer_server)
cmake_minimum_required(VERSION 3.13)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory("api")
add_subdirectory("src")
