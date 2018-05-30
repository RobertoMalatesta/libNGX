cmake_minimum_required(VERSION 3.7)
project(Ngx)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES 
	src/Core/MemBlock.cpp)

set(TEST_FILES
        test/Core/MemBlockTest.cpp
        test/Core/Test.cpp)
include_directories(./include)

#add_library(Ngx OBJECT ${SOURCE_FILES})
add_executable(Ngx ${SOURCE_FILES} ${TEST_FILES})