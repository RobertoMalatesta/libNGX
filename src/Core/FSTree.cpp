#include "Core/Core.h"

using namespace ngx::Core;

int FSEntity::Compare(FSEntity *Node) {

    if (KeyLength > Node->KeyLength) {
        return  1;
    }
    else if (KeyLength < Node->KeyLength) {
        return -1;
    }

    if (Hash > Node->Hash) {
        return 1;
    }
    else if (Hash < Node->Hash) {
        return -1;
    }

    return  strncmp((const char *)Key, (const char *)Node->Key, KeyLength);
}