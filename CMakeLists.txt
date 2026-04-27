cmake_minimum_required(VERSION 3.20)

project(TerminalTactics VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

option(TERMINAL_TACTICS_BUILD_APP "Build ncurses game executable" ON)
option(TERMINAL_TACTICS_BUILD_BENCHMARKS "Build benchmark executables" ON)
option(TERMINAL_TACTICS_BUILD_MT "Build experimental multithreaded executables" OFF)
option(TERMINAL_TACTICS_BUILD_TESTS "Build unit tests" ${BUILD_TESTING})

add_compile_options(-Wall -Wpedantic -Wextra)

add_library(project_headers INTERFACE)

target_include_directories(project_headers
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(matrix)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/libs/*.cpp)
list(FILTER SRC_FILES EXCLUDE REGEX ".*_mt\\.cpp$")
list(FILTER SRC_FILES EXCLUDE REGEX ".*/libs/view/.*\\.cpp$")

add_library(core_lib STATIC ${SRC_FILES})
target_link_libraries(core_lib PUBLIC matrix project_headers)

if(TERMINAL_TACTICS_BUILD_APP)
    find_package(Curses QUIET)
    if(CURSES_FOUND)
        add_library(view_lib STATIC libs/view/NcursesView.cpp)
        target_include_directories(view_lib PRIVATE ${CURSES_INCLUDE_DIRS})
        target_link_libraries(view_lib PUBLIC core_lib project_headers ${CURSES_LIBRARIES})

        add_executable(game_app main.cpp)
        target_include_directories(game_app PRIVATE ${CURSES_INCLUDE_DIRS})
        target_link_libraries(game_app PRIVATE core_lib view_lib project_headers ${CURSES_LIBRARIES})
    else()
        message(WARNING "ncurses was not found; game_app will not be built")
    endif()
endif()

if(TERMINAL_TACTICS_BUILD_BENCHMARKS)
    add_executable(timing timing.cpp)
    target_link_libraries(timing PRIVATE core_lib project_headers)
endif()

if(TERMINAL_TACTICS_BUILD_MT)
    find_package(TBB REQUIRED)
    file(GLOB_RECURSE SRC_MT_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/libs/*_mt.cpp)
    list(FILTER SRC_MT_FILES EXCLUDE REGEX ".*/libs/view/.*\\.cpp$")

    add_library(mt_lib STATIC ${SRC_MT_FILES})
    target_link_libraries(mt_lib PUBLIC matrix project_headers TBB::tbb)

    if(TERMINAL_TACTICS_BUILD_APP)
        if(NOT DEFINED CURSES_FOUND)
            find_package(Curses QUIET)
        endif()
        if(CURSES_FOUND)
            add_library(view_mt_lib STATIC libs/view/NcursesView_mt.cpp)
            target_include_directories(view_mt_lib PRIVATE ${CURSES_INCLUDE_DIRS})
            target_link_libraries(view_mt_lib PUBLIC mt_lib project_headers ${CURSES_LIBRARIES})

            add_executable(game_app_mt main_mt.cpp)
            target_include_directories(game_app_mt PRIVATE ${CURSES_INCLUDE_DIRS})
            target_link_libraries(game_app_mt PRIVATE mt_lib view_mt_lib project_headers ${CURSES_LIBRARIES})
        else()
            message(WARNING "ncurses was not found; game_app_mt will not be built")
        endif()
    endif()

    if(TERMINAL_TACTICS_BUILD_BENCHMARKS)
        add_executable(timing_mt timing_mt.cpp)
        target_link_libraries(timing_mt PRIVATE mt_lib project_headers)
    endif()
endif()

if(TERMINAL_TACTICS_BUILD_TESTS)
    add_subdirectory(tests)
endif()
