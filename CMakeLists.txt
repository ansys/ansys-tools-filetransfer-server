project(ansys_utilities_filetransfer_server)
cmake_minimum_required(VERSION 3.13)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Variables used by the API subdirectory
set(filetransfer_api_libs ${CONAN_LIBS})
if(WIN32)
    set(grpc_cpp_plugin ${CONAN_BIN_DIRS_GRPC}/grpc_cpp_plugin.exe)
else()
    set(grpc_cpp_plugin ${CONAN_BIN_DIRS_GRPC}/grpc_cpp_plugin)
endif()

add_subdirectory("api")
add_subdirectory("src")
