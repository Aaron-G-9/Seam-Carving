cmake_minimum_required(VERSION 2.8.12)

project(seamCarving)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(seamCarving main.cpp)
target_link_libraries(seamCarving ${OpenCV_LIBS})
