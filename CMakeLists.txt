cmake_minimum_required(VERSION 3.7)

project(NgxTest)
project(NgxLibrary)
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
        src/Core/Error.cpp
        src/Core/Pool.cpp
        src/Core/Buffer.cpp
        src/Core/Array.cpp
        src/Core/Queue.cpp
        src/Core/List.cpp
        src/Core/RBTree.cpp
        src/Core/Hash.cpp
        src/Core/ArchX86.cpp
        src/Core/Lock.cpp
        src/Core/Time.cpp
        src/Core/Checksum.cpp
        src/Core/ThreadPool.cpp
        src/Core/TimerTree.cpp
        src/Core/FSTree.cpp
        src/Core/Connection.cpp
        src/Core/EventDomain.cpp
        src/Core/EPollEventDomain.cpp
        src/Core/Server.cpp
        src/Core/SocketEventDomain.cpp
        src/Core/MemoryBlock.cpp
        src/Core/MemoryBlockAllocator.cpp
        src/Core/BufferMemoryBlock.cpp
        src/Core/BufferMemoryBlockRecycler.cpp
        src/Core/BufferBuilder.cpp
        src/Core/Reference.cpp
        src/Http/HttpConnection.cpp
        src/Http/HttpServer.cpp
        src/Http/HttpConnectionRecycler.cpp
        src/Http/HttpRequestContext.cpp
        src/Http/HttpError.cpp
        src/Core/Listening.cpp src/Core/Socket.cpp)

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
        test/Http/HttpConnectionRecyclerTest.cpp)

include_directories(./include)

LINK_LIBRARIES(-lpthread)

add_executable(NgxTest ${SOURCE_FILES} ${TEST_FILES})
add_library(NgxLibrary STATIC ${SOURCE_FILES})