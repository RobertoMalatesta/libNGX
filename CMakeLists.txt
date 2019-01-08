cmake_minimum_required(VERSION 3.7)

project(NGXTest)
project(libNGX)
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/Core/Arch/ArchX86.cpp
        src/Core/Types/Queue.cpp
        src/Core/Types/RBTree.cpp
        src/Core/BasicComponent/Buffer.cpp
        src/Core/BasicComponent/BasicMemoryBlock.cpp
        src/Core/BasicComponent/BufferMemoryBlock.cpp
        src/Core/BasicComponent/BufferMemoryBlockRecycleBin.cpp
        src/Core/BasicComponent/BufferBuilder.cpp
        src/Core/BasicComponent/Checksum.cpp
        src/Core/BasicComponent/Connection.cpp
        src/Core/BasicComponent/Error.cpp
        src/Core/BasicComponent/EventDomain.cpp
        src/Core/BasicComponent/SpinLock.cpp
        src/Core/BasicComponent/Listening.cpp
        src/Core/BasicComponent/MemoryBlockAllocator.cpp
        src/Core/BasicComponent/Pool.cpp
        src/Core/BasicComponent/Reference.cpp
        src/Core/BasicComponent/Socket.cpp
        src/Core/BasicComponent/SocketEventDomain.cpp
        src/Core/BasicComponent/Time.cpp
        src/Core/BasicComponent/ThreadPool.cpp
        src/Core/BasicComponent/SocketTimerHub.cpp
        src/Core/BasicComponent/BufferCursor.cpp
        src/Core/EPollEventDomain.cpp
        src/HTTP/Router/Mux.cpp
        src/HTTP/HTTPListening.cpp
        src/HTTP/HTTPConnection.cpp
        src/HTTP/HTTPServer.cpp
        src/HTTP/HTTPConnectionRecycleBin.cpp
        src/HTTP/HTTPError.cpp
        src/HTTP/HTTPConnectionBuilder.cpp
        src/Core/BasicComponent/Dictionary.cpp
        src/HTTP/HTTPHeader.cpp src/HTTP/HTTPRequest.cpp)

set(TEST_FILES
        test/Test.cpp
        test/Core/MemBlockTest.cpp
        test/Core/PoolTest.cpp
        test/Core/ArrayTest.cpp
        test/Core/TimeTest.cpp
        test/Core/ThreadPool.cpp
        test/Core/HashTest.cpp
        test/Core/CRCTest.cpp
        test/Core/TimerHubTest.cpp
        test/Core/SocketTest.cpp
        test/Core/EPollEventDomainTest.cpp
        test/Core/LockTest.cpp
        test/Core/BufferTest.cpp
        test/Core/CoreTest.cpp
        test/Core/DictionaryTest.cpp
        test/HTTP/HTTPTest.cpp
        test/HTTP/HTTPServerTest.cpp
        test/HTTP/HTTPConnectionRecycleBin.cpp
        test/HTTP/HTTPParseTest.cpp)

include_directories(./include)

LINK_LIBRARIES(-lpthread -lglog)

add_executable(NGXTest ${SOURCE_FILES} ${TEST_FILES})
add_library(libNGX STATIC ${SOURCE_FILES})