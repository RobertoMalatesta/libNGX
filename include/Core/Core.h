#include "ngx.h"

namespace ngx::Core {
    using namespace ngx::Core::DefaultConfig;
    class Reference;
    class Resetable;
    class Recyclable;
    class Recycler;
    class MemoryBlock;
    class MemoryBlockAllocator;
    class MemAllocator;
    class Pool;
    class Array;
    class BufferCursor;
    class BufferRange;
    class Buffer;
    class BufferMemoryBlockRecycler;
    class BufferBuilder;
    class Queue;
    class Error;
    class RuntimeError;
    class SocketError;
    class EventError;
    class List;
    class ListCarriage;
    class ListIterator;
    class RBTreeNode;
    class RBTree;
    class UInt32RBTreeNode;
    class UInt32RBTree;
    class TimerTree;
    class FSEntity;
    class FSTree;
    class Socket;
    class SpinLock;
    class SpinlockGuard;
    class Promise;
    class Thread;
    class ThreadPool;
    class EventDomain;
    struct SocketAddress;
    class SocketEventDomain;
    class EPollEventDomain;
    class Socket;
    class Connection;
    class Listening;
    class TCP4Connection;
    class TCP4Listening;
    class Server;
}

#include "Core/ArchX86.h"
#include "Core/LinuxOSUtility.h"

#include "Core/Resetable.h"
#include "Core/Reference.h"
#include "Core/Queue.h"
#include "Core/Recyclable.h"
#include "Core/Recylcer.h"
#include "Core/MemAllocator.h"
#include "Core/Error.h"

#include "Core/MemoryBlock.h"
#include "Core/MemoryBlockAllocator.h"
#include "Core/Time.h"
#include "Core/Array.h"
#include "Core/List.h"
#include "Core/Hash.h"
#include "Core/Lock.h"
#include "Core/RBTree.h"
#include "Core/Checksum.h"
#include "Core/FSTree.h"

#include "Core/Pool.h"
#include "Core/BufferMemoryBlock.h"
#include "Core/Buffer.h"
#include "Core/BufferAllocator.h"
#include "Core/BufferMemoryBlockRecycler.h"
#include "Core/BufferBuilder.h"
#include "Core/ThreadPool.h"
#include "Core/TimerTree.h"

#include "Core/EventEntity.h"
#include "Core/EventDomain.h"
#include "Core/Socket.h"
#include "Core/Connection.h"
#include "Core/Listening.h"
#include "Core/SocketEventDomain.h"
#include "Core/EPollEventDomain.h"
#include "Core/DAGScheduler.h"
#include "Core/Server.h"
