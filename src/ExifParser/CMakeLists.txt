add_library(ExifParser SHARED
            ExifParser.cpp
            ExifParser.hpp
)

set(EXIF_PARSER_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(CppUTests)