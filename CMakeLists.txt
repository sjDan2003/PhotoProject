cmake_minimum_required(VERSION 3.11.0)
project(PhotoProject VERSION 0.1.0)

include(cmake/doxygen.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)

message(STATUS "CMAKE_CXX_CLANG_TIDY: ${CMAKE_CXX_CLANG_TIDY}")

find_program(iwyu_path NAMES include-what-you-use iwyu REQUIRED)
