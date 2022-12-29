
add_subdirectory(ExifParser)

add_executable(PhotoProject Filesystem.hpp Filesystem.cpp main.cpp)

target_link_libraries(PhotoProject ExifParser ${Boost_LIBRARIES})