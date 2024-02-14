cmake_minimum_required(VERSION 3.16)
project(ansys_tools_filetransfer_server)

add_subdirectory("api")
add_subdirectory("src")
add_subdirectory("test")

enable_testing()
