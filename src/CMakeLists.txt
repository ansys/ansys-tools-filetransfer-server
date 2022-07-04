add_executable(server server.cpp)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/api)
target_link_libraries(server file_transfer_api)
