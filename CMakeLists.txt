cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
 
project(SimpleHandTracking)
 
find_package(OpenCV REQUIRED)
 
add_executable(SimpleHandTracking main.cpp)
 
target_link_libraries (SimpleHandTracking  ${OpenCV_LIBS})
