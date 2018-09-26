#include "ngx.h"

namespace ngx::Core {
    class Ref;
    class MemoryBlock;
    class MemoryBlockAllocator;
    class MemAllocator;
    class Pool;
    class Array;
    class Buffer;
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
    class SpinLock;
    class Promise;
    class Thread;
    class ThreadPool;
    class Connection;
    class Listening;
    class Event;
    class EventDomain;
    class SocketEventDomain;
    class EPollEventDomain;
    class Server;
}

#include "Core/ArchX86.h"
#include "Core/LinuxOSUtility.h"

#include "Core/Time.h"
#include "Core/Error.h"
#include "Core/Ref.h"
#include "Core/Lock.h"
#include "Core/MemAllocator.h"
#include "Core/MemBlock.h"
#include "Core/Pool.h"
#include "Core/Array.h"
#include "Core/Queue.h"
#include "Core/Buffer.h"
#include "Core/List.h"
#include "Core/RBTree.h"
#include "Core/Checksum.h"
#include "Core/Hash.h"

#include "Core/ThreadPool.h"
#include "Core/Connection.h"
#include "Core/TimerTree.h"
#include "Core/FSTree.h"
#include "Core/DAGScheduler.h"
#include "Core/EventDomain.h"
#include "Core/EPollEventDomain.h"
#include "Core/Server.h"