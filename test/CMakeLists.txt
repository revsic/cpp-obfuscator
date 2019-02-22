cmake_minimum_required(VERSION 3.10)
project(unittest)

set(CMAKE_CXX_STANDARD 17)

file(GLOB test_files 
        "impl/*.cpp"
)

add_executable(unittest main.cpp ${test_files})
target_include_directories(unittest PRIVATE ../obfuscator)

add_subdirectory(../external/Catch2 external)
target_link_libraries(unittest Catch2::Catch2)
