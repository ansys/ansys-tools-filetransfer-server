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

add_executable(server server.cpp)
target_link_libraries(server filetransfer_service)
target_link_libraries(server ${CONAN_LIBS})
