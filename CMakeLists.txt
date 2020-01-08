cmake_minimum_required(VERSION 3.12)
project(myshell)

set(CMAKE_CXX_STANDARD 17)

FIND_PACKAGE(Boost 1.69 COMPONENTS program_options REQUIRED)
FIND_PACKAGE(Boost 1.69 COMPONENTS filesystem REQUIRED)

add_executable(myshell main.cpp modules.h utils.h)
add_executable(myls myls.cpp)
add_executable(mycat mycat.cpp)
add_executable(mymv mymv.cpp)
add_executable(myrm myrm.cpp)
add_executable(mymkdir mymkdir.cpp)

target_link_libraries(myshell Boost::program_options Boost::filesystem)
target_link_libraries(myls Boost::program_options Boost::filesystem)
target_link_libraries(mycat Boost::program_options)
target_link_libraries(mymv Boost::filesystem)
target_link_libraries(myrm Boost::filesystem)
target_link_libraries(mymkdir Boost::filesystem)