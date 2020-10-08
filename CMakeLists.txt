cmake_minimum_required(VERSION 3.0.0)
project(PhotoProject VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)

add_compile_options(-Wall -Werror)

add_subdirectory(src)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
