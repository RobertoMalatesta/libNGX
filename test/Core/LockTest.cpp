#include "Core/Core.h"

using namespace ngx::Core;

int LockTest() {
    BigSpinlock Lock;

    Lock.Lock();
    Lock.Unlock();

    return 0;
}
