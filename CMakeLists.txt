cmake_minimum_required(VERSION 3.23)
project(breakout)

set(CMAKE_CXX_STANDARD 14)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_executable(breakout
        game/main.h
        main.cpp)

target_link_libraries(breakout ${CURSES_LIBRARIES})