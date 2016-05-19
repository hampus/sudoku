project(sudoku CXX)
cmake_minimum_required(VERSION 3.1)

add_executable(solver main.cpp)
set_property(TARGET solver PROPERTY CXX_STANDARD 14)
