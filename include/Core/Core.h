#include "ngx.h"

namespace ngx::Core {
    using namespace ngx::Core::DefaultConfig;
    class Ref;
    class Resetable;
    class Recyclable;
    class Recycler;
    class MemoryBlock;
    class MemoryBlockAllocator;
    class MemAllocator;
    class Pool;
    class Array;
    class Buffer;
    class BufferMemoryBlockRecycler;
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
    class Promise;
    class Thread;
    class ThreadPool;
    class EventDomain;
    class SocketEventDomain;
    class EPollEventDomain;
    class Connection;
    class Listening;
    class TCP4Connection;
    class TCP4Listening;
    class Server;
}

#include "Core/ArchX86.h"
#include "Core/LinuxOSUtility.h"

#include "Core/Time.h"
#include "Core/Ref.h"
#include "Core/Error.h"
#include "Core/Queue.h"
#include "Core/Array.h"
#include "Core/List.h"
#include "Core/Hash.h"
#include "Core/Recylcer.h"
#include "Core/Lock.h"
#include "Core/RBTree.h"
#include "Core/Checksum.h"

#include "Core/MemAllocator.h"
#include "Core/MemBlock.h"
#include "Core/Pool.h"
#include "Core/Buffer.h"
#include "Core/ThreadPool.h"
#include "Core/TimerTree.h"
#include "Core/FSTree.h"

#include "Core/EventDomain.h"
#include "Core/Socket.h"
#include "Core/SocketEventDomain.h"
#include "Core/EPollEventDomain.h"
#include "Core/Connection.h"
#include "Core/DAGScheduler.h"
#include "Core/Server.h"