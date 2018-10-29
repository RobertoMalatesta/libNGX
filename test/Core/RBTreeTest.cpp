#include "Core/Core.h"

using namespace ngx::Core;


int RBTreeTest() {

    Pool MemoryPool;

    UInt32RBTree *RBTree = UInt32RBTree::CreateFromAllocator(&MemoryPool, &MemoryPool);

    UInt32RBTreeNode *Temp;

    std::vector<UInt32RBTreeNode *> Nodes;

    Nodes.clear();

    for (uint32_t i = 0; i < 10000; i++) {
        Temp = RBTree->CreateNodeFromAllocator(1, i);
        Nodes.push_back(Temp);
        RBTree->Insert(Temp);
    }

    UInt32RBTreeNode *Node;

    uint32_t Sum = 0;

    for (uint32_t i = 0; i < 10001; i++) {
        Node = RBTree->Find(i);

        if (Node != nullptr) {
            Sum += Node->GetKey();
        }
    }

    for (RBTreeNode *N = RBTree->Minimum(); N != nullptr; N = RBTree->Next(N)) {
        printf("%u\n", ((UInt32RBTreeNode *) N)->GetKey());
    }


    for (UInt32RBTreeNode *it : Nodes) {
        Node = it;
        RBTree->Delete(Node);
        RBTree->FreeNodeFromAllocator(&Node);
    }

    Nodes.clear();

    for (int i = 0; i < 10000; i++) {
        Temp = RBTree->CreateNodeFromAllocator(1, (uint32_t) i);
        Nodes.push_back(Temp);
        RBTree->Insert(Temp);
    }

    for (UInt32RBTreeNode *it : Nodes) {
        Node = it;
        RBTree->Delete(Node);
        RBTree->FreeNodeFromAllocator(&Node);

    }

    UInt32RBTree::FreeFromAllocator(&MemoryPool, &RBTree);

    MemoryPool.GC();

    return 0;
}