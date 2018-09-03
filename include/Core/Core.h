#include "ngx.h"

namespace ngx::Core {
    class Ref;
    class MemBlock;
    class MemAllocator;
    class Pool;
    class Array;
    class Buf;
    class Queue;
    class Error;
    class RuntimeError;
    class SocketError;
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
    class Promise;
    class Thread;
    class ThreadPool;
    class Connection;
    class Listening;
    class Event;
    class EventDomain;

}

#include "Core/ArchX86.h"
#include "Core/LinuxOSUtility.h"

#include "Core/Time.h"
#include "Core/Error.h"
#include "Core/Ref.h"
#include "Core/MemAllocator.h"
#include "Core/MemBlock.h"
#include "Core/Pool.h"
#include "Core/Array.h"
#include "Core/Buf.h"
#include "Core/Queue.h"
#include "Core/List.h"
#include "Core/RBTree.h"
#include "Core/Checksum.h"
#include "Core/Hash.h"


#include "Core/Connection.h"
#include "Core/Listening.h"
#include "Core/ThreadPool.h"
#include "Core/TimerTree.h"
#include "Core/FSTree.h"
#include "Core/EventDomain.h"
#include "Core/EPollEventDomain.h"