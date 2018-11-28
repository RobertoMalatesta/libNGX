#include "Core/Core.h"

using namespace ngx::Core;


int RBTree2Test() {

    UInt32RBT HashTree;

    UInt32RBNode Node1(1);
    UInt32RBNode Node2(2);
    UInt32RBNode Node3(3);
    UInt32RBNode Node4(4);
    UInt32RBNode Node5(5);


    HashTree.Insert(&Node1);
    HashTree.Insert(&Node2);
    HashTree.Insert(&Node3);
    HashTree.Insert(&Node4);
    HashTree.Insert(&Node5);

    for (RBNode *It = HashTree.Begin(); It; It = HashTree.Next(It)) {
        printf("%u\n", ((UInt32RBNode *)It)->GetKey());
        HashTree.Erase(It);
    }

    return 0;
}
