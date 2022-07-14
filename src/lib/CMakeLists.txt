include_directories(${CMAKE_CURRENT_BINARY_DIR}/api)

add_library(
    filetransfer_service
    STATIC
    filetransfer_service_upload.cpp
    filetransfer_service_download.cpp
    sha1_digest.cpp
    exception_handling.cpp
)
target_link_libraries(filetransfer_service file_transfer_api)
target_link_libraries(filetransfer_service ${CONAN_LIBS})
target_include_directories(filetransfer_service PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
