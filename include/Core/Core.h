#include "ngx.h"

namespace ngx::Core {
    class Ref;
    class MemBlock;
    class MemAllocator;
    class Pool;
    class Array;
    class Buf;
    class Queue;
    class List;
    class ListCarriage;
    class ListIterator;
    class RBTreeNode;
    class RBTree;
    class UInt32RBTreeNode;
    class UInt32RBTree;
    class Promise;
    class ThreadPool;
    class Connection;
    class Listening;

}

#include "Core/ArchX86.h"
#include "Core/LinuxOSUtility.h"

#include "Core/Time.h"

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