#include "Core/Core.h"

using namespace std;
using namespace ngx::Core;

int FSTreeTest () {

    Pool pool;
    FSTree * Tree = FSTree::CreateFromAllocator(&pool, &pool);
    FSEntity *Child;
    u_char buf[120], *PointerToData;

    const int SIZE = 1000, ROUND = 100000;

    for(int i=0; i<SIZE; i++) {

        sprintf((char *)buf, "节点%d", i);
        Child = Tree->CreateChild(buf, strlen((char *)buf), 80, true);
        PointerToData = Child->GetDataPointer();
        sprintf((char *)PointerToData, "节点%d中存储的数据", i);
    }

    for (int r=0; r<ROUND; r++) {
        for(int i=0; i<SIZE; i++) {

            sprintf((char *)buf, "节点%d", i);
            Child = Tree->QueryChild(buf, strlen((char *)buf), true);

            if (nullptr != Child) {
//                printf("%s\n", Child->GetDataPointer());
            }
            else {
                printf("[BUG] not found!\n");
            }
        }
    }

    for(int i=0; i<SIZE; i++) {

        sprintf((char *)buf, "节点%d", i);
        Tree->DeleteChild(buf, strlen((char *)buf), true);
    }

    FSTree::FreeFromAllocator(&pool, &Tree);

    pool.GC();

    return 0;
}
