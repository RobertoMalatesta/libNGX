#include "Core/Core.h"

using namespace std;
using namespace ngx::Core;

int FSTreeTest () {

    Pool pool;

    FSTree * Tree = FSTree::CreateFromAllocator(&pool, &pool);

    FSEntity *Child;

    u_char *PointerToData;

    Child = Tree->CreateChild((u_char *)"节点1", sizeof("节点1"), 80, true);
    PointerToData = Child->GetDataPointer();
    sprintf((char *)PointerToData, "节点1中存储的数据");

    Child = Tree->CreateChild((u_char *) "节点2", sizeof("节点2"), 80, true);
    PointerToData = Child->GetDataPointer();
    sprintf((char *)PointerToData, "节点2中存储的数据");

    Child = Tree->CreateChild((u_char *) "节点3", sizeof("节点3"), 80, true);
    PointerToData = Child->GetDataPointer();
    sprintf((char *)PointerToData, "节点3中存储的数据");

    Child = Tree->CreateChild((u_char *) "节点4", sizeof("节点4"), 80, true);
    PointerToData = Child->GetDataPointer();
    sprintf((char *)PointerToData, "节点4中存储的数据");

    Child = Tree->CreateChild((u_char *) "节点5", sizeof("节点5"), 80, true);
    PointerToData = Child->GetDataPointer();
    sprintf((char *)PointerToData, "节点5中存储的数据");


    Child = nullptr;
    Child = Tree->QueryChild((u_char *)"节点1", sizeof("节点1"), true);

    if (nullptr != Child) {
        printf("%s\n", Child->GetDataPointer());
    }

    Child = Tree->QueryChild((u_char *)"节点2", sizeof("节点2"), true);

    if (nullptr != Child) {
        printf("%s\n", Child->GetDataPointer());
    }

    Child = Tree->QueryChild((u_char *)"节点3", sizeof("节点3"), true);

    if (nullptr != Child) {
        printf("%s\n", Child->GetDataPointer());
    }

    Child = Tree->QueryChild((u_char *)"节点4", sizeof("节点4"), true);

    if (nullptr != Child) {
        printf("%s\n", Child->GetDataPointer());
    }

    Child = Tree->QueryChild((u_char *)"节点5", sizeof("节点5"), true);

    if (nullptr != Child) {
        printf("%s\n", Child->GetDataPointer());
    }

    Tree->DeleteChild((u_char *)"节点1", sizeof("节点1"), true);
    Tree->DeleteChild((u_char *)"节点2", sizeof("节点2"), true);
    Tree->DeleteChild((u_char *)"节点3", sizeof("节点3"), true);
    Tree->DeleteChild((u_char *)"节点4", sizeof("节点4"), true);
    Tree->DeleteChild((u_char *)"节点5", sizeof("节点5"), true);

    FSTree::FreeFromAllocator(&pool, &Tree);

    return 0;
}
