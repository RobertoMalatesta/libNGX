cmake_minimum_required(VERSION 3.7)

project(NgxTest)
project(NgxLibrary)
set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
        src/Core/MemBlock.cpp
        src/Core/Pool.cpp
        src/Core/Buf.cpp
        src/Core/Array.cpp
        src/Core/Queue.cpp
        src/Core/List.cpp
        src/Core/RBTree.cpp
        src/Core/CryptoUtil.cpp
        src/Core/Arch.cpp)

set(TEST_FILES
        test/Core/Test.cpp
        test/Core/MemBlockTest.cpp
        test/Core/PoolTest.cpp
        test/Core/BufTest.cpp
        test/Core/ArrayTest.cpp)
include_directories(./include)

add_executable(NgxTest ${SOURCE_FILES} ${TEST_FILES})
add_library(NgxLibrary STATIC ${SOURCE_FILES})