cmake_minimum_required(VERSION 3.28.3)

project(MyGameProject VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(--coverage)
add_link_options(--coverage)

#add_subdirectory(src)
add_subdirectory(matrix)
add_subdirectory(tests)