cmake_minimum_required(VERSION 3.16)

project(Jade)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(DSA)
add_subdirectory(IR)
add_subdirectory(analysis)
add_subdirectory(passes)

add_subdirectory(tests)
