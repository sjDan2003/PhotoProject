
add_subdirectory(ExifParser)

add_executable(PhotoProject Filesystem.hpp Filesystem.cpp main.cpp)
target_link_libraries(PhotoProject PRIVATE ExifParser)
target_include_directories(PhotoProject PRIVATE ExifParser)

#set_property(TARGET PhotoProject PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})