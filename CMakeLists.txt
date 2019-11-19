cmake_minimum_required(VERSION 3.12)
project(myshell)

set(CMAKE_CXX_STANDARD 17)

FIND_PACKAGE(Boost 1.69 COMPONENTS program_options REQUIRED)
FIND_PACKAGE(Boost 1.69 COMPONENTS filesystem REQUIRED)

add_executable(myshell main.cpp)
add_executable(myls myls.cpp)

target_link_libraries(myshell Boost::program_options Boost::filesystem)
target_link_libraries(myls Boost::program_options Boost::filesystem)