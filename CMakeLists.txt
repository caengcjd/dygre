cmake_minimum_required (VERSION 2.8)
include(CheckCXXCompilerFlag)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
project (dygre)

aux_source_directory(./src DIR_SRCS)

link_libraries(jsoncpp yaml-cpp uuid)

add_executable(dygre ${DIR_SRCS})
