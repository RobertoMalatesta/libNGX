#include "Core/Core.h"

using namespace ngx::Core;


int RBTreeTest() {

    Pool MemoryPool;

    UInt32RBTree *RBTree = UInt32RBTree::CreateFromAllocator(&MemoryPool, &MemoryPool);

    UInt32RBTreeNode *Temp;

    vector<UInt32RBTreeNode *> Nodes;

    Nodes.clear();

    for (int i = 0 ; i< 10000; i++) {
        Temp = RBTree->CreateNodeFromAllocator(1, (uint32_t)i);
        Nodes.push_back(Temp);
        RBTree->Insert(Temp);
    }

    for (UInt32RBTreeNode *it : Nodes) {
        UInt32RBTreeNode *Node = it;
        RBTree->Delete(Node);
        RBTree->FreeNodeFromAllocator(&Node);

    }

    Nodes.clear();

    for (int i = 0 ; i< 10000; i++) {
        Temp = RBTree->CreateNodeFromAllocator(1, (uint32_t)i);
        Nodes.push_back(Temp);
        RBTree->Insert(Temp);
    }

    for (UInt32RBTreeNode *it : Nodes) {
        UInt32RBTreeNode *Node = it;
        RBTree->Delete(Node);
        RBTree->FreeNodeFromAllocator(&Node);

    }

    UInt32RBTree::FreeFromAllocator(&MemoryPool, &RBTree);

    MemoryPool.GC();

    return 0;

}