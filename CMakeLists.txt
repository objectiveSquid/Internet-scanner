cmake_minimum_required(VERSION 3.10)

project(InternetScanner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SRC_FILES src/*.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

add_executable(
    InternetScanner
    ${SRC_FILES}
)

target_include_directories(InternetScanner PRIVATE include)
target_include_directories(InternetScanner PRIVATE .argparse)

set(CMAKE_BUILD_TYPE Release)
