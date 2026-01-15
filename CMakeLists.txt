cmake_minimum_required(VERSION 3.28.3)

project(MyGameProject VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#add_compile_options(--coverage)
#add_link_options(--coverage)

#add_compile_options(-fsanitize=thread -g)
#add_link_options(-fsanitize=thread -g)

add_compile_options(-Wall -Wpedantic -Wextra)

add_library(project_headers INTERFACE)

target_include_directories(project_headers
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/libs/*.cpp)
list(FILTER SRC_FILES EXCLUDE REGEX ".*_mt\\.cpp$")
file(GLOB_RECURSE SRC_MT_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/libs/*_mt.cpp)

add_library(core_lib STATIC ${SRC_FILES})

add_library(mt_lib STATIC ${SRC_MT_FILES})

find_package(TBB REQUIRED)

target_link_libraries(core_lib PUBLIC matrix project_headers)
target_link_libraries(mt_lib PUBLIC matrix project_headers TBB::tbb)

find_package(Curses REQUIRED)
add_executable(game_app main.cpp)
target_include_directories(game_app PRIVATE ${CURSES_INCLUDE_DIRS})
target_link_libraries(game_app PRIVATE core_lib project_headers ${CURSES_LIBRARIES})

add_executable(game_app_mt main_mt.cpp)
target_include_directories(game_app_mt PRIVATE ${CURSES_INCLUDE_DIRS})
target_link_libraries(game_app_mt PRIVATE mt_lib project_headers ${CURSES_LIBRARIES})

add_executable(timing timing.cpp)
target_link_libraries(timing PRIVATE core_lib project_headers)

add_executable(timing_mt timing_mt.cpp)
target_link_libraries(timing_mt PRIVATE mt_lib project_headers)

#add_subdirectory(libs)
add_subdirectory(matrix)
add_subdirectory(tests)
