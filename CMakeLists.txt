cmake_minimum_required(VERSION 3.7)

project(NGXTest)
project(libNGX)
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/Core/Array.cpp
        src/Core/ArchX86.cpp
        src/Core/Buffer.cpp
        src/Core/BufferMemoryBlock.cpp
        src/Core/BufferMemoryBlockRecycler.cpp
        src/Core/BufferBuilder.cpp
        src/Core/Checksum.cpp
        src/Core/Connection.cpp
        src/Core/EPollEventDomain.cpp
        src/Core/Error.cpp
        src/Core/EventDomain.cpp
        src/Core/FSTree.cpp
        src/Core/Hash.cpp
        src/Core/List.cpp
        src/Core/LinuxOSUtility.cpp
        src/Core/Lock.cpp
        src/Core/Listening.cpp
        src/Core/Time.cpp
        src/Core/ThreadPool.cpp
        src/Core/TimerTree.cpp
        src/Core/MemoryBlock.cpp
        src/Core/MemoryBlockAllocator.cpp
        src/Core/Pool.cpp
        src/Core/Queue.cpp
        src/Core/RBTree.cpp
        src/Core/Reference.cpp
        src/Core/Server.cpp
        src/Core/Socket.cpp
        src/Core/SocketEventDomain.cpp
        src/Core/StringUtil.cpp
        src/Http/HttpConnection.cpp
        src/Http/HttpServer.cpp
        src/Http/HttpConnectionRecycler.cpp
        src/Http/HttpRequestContext.cpp
        src/Http/HttpError.cpp
        )

set(TEST_FILES
        test/Test.cpp
        test/Core/MemBlockTest.cpp
        test/Core/PoolTest.cpp
        test/Core/BufTest.cpp
        test/Core/ArrayTest.cpp
        test/Core/TimeTest.cpp
        test/Core/ThreadPool.cpp
        test/Core/HashTest.cpp
        test/Core/CRCTest.cpp
        test/Core/RBTreeTest.cpp
        test/Core/TimerTreeTest.cpp
        test/Core/FSTreeTest.cpp
        test/Core/SocketTest.cpp
        test/Core/EPollEventDomainTest.cpp
        test/Core/LockTest.cpp
        test/Core/BufferTest.cpp
        test/Core/CoreTest.cpp
        test/Http/HttpServerTest.cpp
        test/Http/HttpTest.cpp
        test/Http/HttpConnectionRecycler.cpp)

include_directories(./include)

LINK_LIBRARIES(-lpthread)

add_executable(NGXTest ${SOURCE_FILES} ${TEST_FILES})
add_library(libNGX STATIC ${SOURCE_FILES})