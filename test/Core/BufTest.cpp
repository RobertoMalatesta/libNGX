#include "Core/Core.h"

using namespace std;
using namespace ngx::Core;

int BufTest () {

    Pool MemPool(8192);
    Buf New_Buf(&MemPool , 200);
    return 0;
}
