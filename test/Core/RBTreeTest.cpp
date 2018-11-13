#include "Core/Core.h"

using namespace ngx::Core;


int RBTreeTest() {

    Pool MemoryPool;

    UInt32RBTree RBTree(&MemoryPool);

    UInt32RBTreeNode *Temp;

    std::vector<UInt32RBTreeNode *> Nodes;

    Nodes.clear();

    for (uint32_t i = 0; i < 10000; i++) {
        RBTree.Build(Temp);
        Temp->SetKey(i);
        Temp->Value.UInt = i;
        RBTree.Insert(Temp);
        Nodes.push_back(Temp);
    }

    UInt32RBTreeNode *Node;

    uint32_t Sum = 0;

    for (uint32_t i = 0; i < 10001; i++) {
        Node = RBTree.Find(i);
    }

    for (RBTreeNode *N = RBTree.Minimum(); N != nullptr; N = RBTree.Next(N)) {
        printf("%u\n", ((UInt32RBTreeNode *) N)->Value.UInt);
    }


    for (UInt32RBTreeNode *it : Nodes) {
        Node = it;
        RBTree.Delete(Node);
        RBTree.Destroy(Node);
    }

    Nodes.clear();

    for (uint32_t i = 0; i < 10000; i++) {
        RBTree.Build(Temp);
        Temp->SetKey(i);
        Temp->Value.UInt = i;
        RBTree.Insert(Temp);
        Nodes.push_back(Temp);
    }

    for (UInt32RBTreeNode *it : Nodes) {
        Node = it;
        RBTree.Delete(Node);
        RBTree.Destroy(Node);
    }

    MemoryPool.GC();

    return 0;
}