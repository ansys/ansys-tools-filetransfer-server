project(ansys_utilities_filetransfer_server)
cmake_minimum_required(VERSION 3.13)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

if(WIN32)
    # The boost library we get from Conan is compiled for Windows 7+
    # If we don't specify the BOOST_USE_WINAPI_VERSION, there will be
    # undefined symbols when linking the final binary.
    add_definitions(-DBOOST_USE_WINAPI_VERSION=_WIN32_WINNT_WIN7)
endif()
add_definitions(-DBOOST_STACKTRACE_LINK)

# Variables used by the API subdirectory
set(filetransfer_api_libs ${CONAN_LIBS})
if(WIN32)
    set(grpc_cpp_plugin ${CONAN_BIN_DIRS_GRPC}/grpc_cpp_plugin.exe)
else()
    set(grpc_cpp_plugin ${CONAN_BIN_DIRS_GRPC}/grpc_cpp_plugin)
endif()

add_subdirectory("api")
add_subdirectory("src")
