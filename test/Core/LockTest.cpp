#include "Core/Core.h"

using namespace ngx::Core;

int LockTest() {
    Spinlock Lock;

    Lock.Lock();
    Lock.Unlock();

    return 0;
}
