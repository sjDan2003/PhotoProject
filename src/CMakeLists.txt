add_subdirectory(ExifParser)

add_executable(PhotoProject main.cpp)

target_link_libraries(PhotoProject ExifParser)