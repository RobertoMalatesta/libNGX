cmake_minimum_required(VERSION 3.7)

project(libNGX)
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/Core/ADT/RBTree.cpp
        src/Core/Support/Buffer.cpp
        src/Core/Support/BasicMemoryBlock.cpp
        src/Core/Support/BufferMemoryBlock.cpp
        src/Core/Support/BufferMemoryBlockCollector.cpp
        src/Core/Support/BufferBuilder.cpp
        src/Core/Support/Checksum.cpp
        src/Core/Support/Connection.cpp
        src/Core/Support/Error.cpp
        src/Core/Support/EventDomain.cpp
        src/Core/Support/Listen.cpp
        src/Core/Support/Pool.cpp
        src/Core/Support/Time.cpp
        src/Core/Support/ThreadPool.cpp
        src/Core/Support/TimerTree.cpp
        src/Core/Support/BoundCursor.cpp
        src/Core/Support/Dictionary.cpp
        src/Core/Support/Cursor.cpp
        src/Core/Support/TCPConnection.cpp
        src/Core/Support/TCPListen.cpp
        src/Core/Support/MemoryBuffer.cpp
        src/Core/Target/X86.cpp
        src/Core/Target/Linux/EPollEventDomain.cpp
        src/HTTP/HTTPError.cpp
        src/HTTP/HTTPHeader.cpp
        src/HTTP/HTTPRequest.cpp
        src/HTTP/HTTPResponse.cpp
        src/HTTP/HTTPListen.cpp
        src/HTTP/HTTPConnection.cpp
        src/HTTP/HTTPContext.cpp
        src/HTTP/HTTPComponent.cpp
        )
set(TEST_FILES
        test/Test.cpp
        test/Core/SmallVectorTest.cpp
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
        test/HTTP/HTTPConnectionCollector.cpp
        test/HTTP/HTTPParseTest.cpp)
include_directories(./include)
link_libraries(pthread dl gtest)
add_executable(NGXTest ${SOURCE_FILES} ${TEST_FILES})
add_library(NGX STATIC ${SOURCE_FILES})
add_library(testMux SHARED
        src/HTTP/HTTPComponent.cpp
        test/testMux.cpp)
