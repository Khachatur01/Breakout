cmake_minimum_required(VERSION 3.23)
project(breakout)

set(CMAKE_CXX_STANDARD 20)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_library(breakout_logic STATIC game/Breakout.cpp)

add_executable(breakout main.cpp)

target_link_libraries(breakout ${CURSES_LIBRARIES} breakout_logic)