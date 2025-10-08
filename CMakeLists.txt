CMAKE_MINIMUM_REQUIRED(VERSION 3.27)

PROJECT(dedetracker, C)

include_directories(src)

add_executable(dedetracker src/main.c)
add_executable(test src/test.c)

