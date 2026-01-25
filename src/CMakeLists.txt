find_package(Boost REQUIRED COMPONENTS filesystem locale log program_options stacktrace)

if(WIN32)
    find_package(Boost REQUIRED COMPONENTS stacktrace_windbg)

    # The boost library we get from Conan is compiled for Windows 7+
    # If we don't specify the BOOST_USE_WINAPI_VERSION, there will be
    # undefined symbols when linking the final binary.
    # _WIN32_WINNT=0x0601 corresponds to Windows 7
    add_definitions(-DBOOST_USE_WINAPI_VERSION=0x0601)
else()
    find_package(Boost REQUIRED COMPONENTS stacktrace_addr2line)
endif()

set(CMAKE_CXX_STANDARD 20)

if(MSVC)
    add_compile_options(
        /W4 /WX
        /IGNORE:C4068
    )
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    add_link_options(-static)
endif()

add_subdirectory("lib")
include_directories("vendored")

add_executable(server server.cpp)
target_link_libraries(server PUBLIC filetransfer_service)
target_link_libraries(server PUBLIC Boost::filesystem Boost::locale Boost::log Boost::program_options)
