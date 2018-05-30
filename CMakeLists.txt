cmake_minimum_required(VERSION 3.7)
project(Ngx)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES 
	src/Core/MemBlock.cpp
		src/Core/Pool.cpp)

set(TEST_FILES
        test/Core/Test.cpp
        test/Core/MemBlockTest.cpp
        test/Core/PoolTest.cpp)
include_directories(./include)

add_executable(Ngx ${SOURCE_FILES} ${TEST_FILES})