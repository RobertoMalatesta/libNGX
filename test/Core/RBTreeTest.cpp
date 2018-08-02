#include "Core/Core.h"

using namespace ngx::Core;


int RBTreeTest() {

    Pool MemoryPool;
    RBTreeNode *Node=UInt32RBTreeNode::CreateFromAllocator(&MemoryPool, 10);
    UInt32RBTreeNode::FreeFromAllocator(&MemoryPool, &Node);

    MemoryPool.GC();

//    RBTree *Tree = RBTree::(&MemoryPool);
//
//
    return 0;

}