set(Boost_INCLUDE_DIR /home/sjdan2003/Programming/CPP/boost_1_76_0)
set(Boost_LIBRARY_DIR /home/sjdan2003/Programming/CPP/boost_1_76_0/stage/lib)
find_package(Boost 1.76.0 COMPONENTS filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

add_subdirectory(ExifParser)

add_executable(PhotoProject main.cpp)

target_link_libraries(PhotoProject ExifParser ${Boost_LIBRARIES})