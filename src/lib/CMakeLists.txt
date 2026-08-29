find_package(Boost REQUIRED COMPONENTS filesystem stacktrace)

if(WIN32)
    find_package(Boost REQUIRED COMPONENTS stacktrace_windbg)
else()
    find_package(Boost REQUIRED COMPONENTS stacktrace_addr2line)
endif()

add_definitions(-DBOOST_STACKTRACE_LINK)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/api)

add_library(
    filetransfer_service
    STATIC
    filetransfer_service_upload.cpp
    filetransfer_service_download.cpp
    filetransfer_service_delete.cpp
    filetransfer_service_info.cpp
    sha1_digest.cpp
    exception_handling.cpp
)
target_link_libraries(filetransfer_service PUBLIC file_transfer_api)
target_link_libraries(filetransfer_service PUBLIC Boost::log)
target_link_libraries(filetransfer_service PUBLIC Boost::filesystem)
target_link_libraries(filetransfer_service PUBLIC Boost::stacktrace)

if(WIN32)
    target_link_libraries(filetransfer_service PUBLIC Boost::stacktrace_windbg)
else()
    target_link_libraries(filetransfer_service PUBLIC Boost::stacktrace_addr2line)
endif()

target_include_directories(filetransfer_service PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
