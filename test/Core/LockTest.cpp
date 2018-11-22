#include "Core/Core.h"

using namespace ngx::Core;

int LockTest() {
    SpinLock Lock;

    Lock.Lock();
    Lock.Unlock();

    return 0;
}
