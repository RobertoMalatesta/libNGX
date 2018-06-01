cmake_minimum_required(VERSION 3.7)
project(Ngx)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES 
		src/Core/MemBlock.cpp
		src/Core/Pool.cpp
		src/Core/Buf.cpp
        src/Core/Array.cpp)

set(TEST_FILES
        test/Core/Test.cpp
        test/Core/MemBlockTest.cpp
        test/Core/PoolTest.cpp test/Core/BufTest.cpp test/Core/ArrayTest.cpp include/common_header.h)
include_directories(./include)

add_executable(Ngx ${SOURCE_FILES} ${TEST_FILES})