cmake_minimum_required(VERSION 3.17)
project(FlexLibrary C)

set(CMAKE_C_STANDARD 11)

add_executable(FlexLibrary main.c)
target_link_libraries(FlexLibrary ncurses)