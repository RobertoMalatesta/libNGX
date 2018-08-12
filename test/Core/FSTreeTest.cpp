#include "Core/Core.h"

using namespace std;
using namespace ngx::Core;

int FSTreeTest () {

    Pool pool;

    FSTree * Tree = FSTree::CreateFromAllocator(&pool, &pool);

    FSEntity *Child = Tree->CreateChild((u_char *)"节点1", sizeof("节点1"), 80, true);

    u_char  * PointerToData = Child->GetDataPointer();

    sprintf((char *)PointerToData, "节点1中存储的数据");

    Child = nullptr;

    Child = Tree->QueryChild((u_char *)"节点1", sizeof("节点1"), true);

    if (nullptr != Child) {
        printf("%s", Child->GetDataPointer());
    }

    Tree->DeleteChild((u_char *)"节点1", sizeof("节点1"), true);


    FSTree::FreeFromAllocator(&pool, &Tree);

    return 0;
}
