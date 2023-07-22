include(FetchContent)
FetchContent_Declare(
  cpputest
  GIT_REPOSITORY https://github.com/cpputest/cpputest.git
  GIT_TAG v4.0
)

FetchContent_MakeAvailable(cpputest)

set(TEST_FILES  AllTests.cpp
                ExifParserTests.cpp)

add_executable(ExifParserTests ${TEST_FILES})

target_link_libraries(ExifParserTests PRIVATE ExifParser CppUTest)
target_include_directories(ExifParserTests PRIVATE ${cpputest_SOURCE_DIR}/include)

