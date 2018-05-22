cmake_minimum_required(VERSION 3.7)
project(Ngx)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES 
	src/test.cpp)
include_directories(./include)
add_executable(Ngx ${SOURCE_FILES})

