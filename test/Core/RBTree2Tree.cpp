#include "Core/Core.h"

using namespace ngx::Core;


int RBTree2Test() {

    UInt32RBT HashTree;

    for (uint32_t i = 0; i < 1000000; i++) {
        HashTree.Insert(new UInt32RBNode(i));
    }

    for (RBNode *It = HashTree.Begin(); It; It = HashTree.Next(It)) {
        UInt32RBNode *Node = (UInt32RBNode *)It;

        if (Node->GetKey() % 3 == 0) {
//            printf("%u\n", Node->GetKey());
            HashTree.Erase(It);
            delete (UInt32RBNode *)It;
        }
    }

    for (RBNode *It = HashTree.Begin(); It; It = HashTree.Next(It)) {
        UInt32RBNode *Node = (UInt32RBNode *)It;
//        printf("%u\n", Node->GetKey());
        HashTree.Erase(It);
        delete (UInt32RBNode *)It;
    }

    return 0;
}
