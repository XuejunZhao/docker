cmake_minimum_required(VERSION 3.5)
project(work)

set(CMAKE_CXX_STANDARD 14)

add_executable(work main.cpp attribute.cpp attribute.h tool.h consistency.cpp consistency.h inoutf.h inoutf.cpp)