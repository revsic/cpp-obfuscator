cmake_minimum_required(VERSION 3.10)
project(sample)

set(CMAKE_CXX_STANDARD 17)
add_executable(string_obfs string_obfs.cpp)
add_executable(state_machine state_machine.cpp)
add_executable(random random.cpp)
