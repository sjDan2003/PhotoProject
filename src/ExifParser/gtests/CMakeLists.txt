
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.11.0
)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

add_executable(
  ExifParserTests
  ExifParserTests.cpp
  ../ExifParser.hpp
)
target_link_libraries(
    ExifParserTests
    gtest_main
    ExifParser
)

include(GoogleTest)
gtest_discover_tests(ExifParserTests)
