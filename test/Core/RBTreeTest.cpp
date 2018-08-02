#include "Core/Core.h"

using namespace ngx::Core;


int RBTreeTest() {

    Pool MemoryPool;

    UInt32RBTree *RBTree = UInt32RBTree::CreateFromAllocator(&MemoryPool, &MemoryPool);

    UInt32RBTreeNode *Node = RBTree->CreateNodeFromAllocator(1);
    UInt32RBTreeNode *Node1 = RBTree->CreateNodeFromAllocator(1);
    Node->SetKey(1);
    Node1->SetKey(2);

    RBTree->Insert(Node);
    RBTree->Insert(Node1);

    UInt32RBTree::FreeFromAllocator(&MemoryPool, &RBTree);


//    RBTreeNode *Node=UInt32RBTreeNode::CreateFromAllocator(&MemoryPool, 10);
//    UInt32RBTreeNode::FreeFromAllocator(&MemoryPool, &Node);

    MemoryPool.GC();

//    RBTree *Tree = RBTree::(&MemoryPool);
//
//
    return 0;

}