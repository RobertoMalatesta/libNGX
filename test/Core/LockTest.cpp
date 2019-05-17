#include "Core/Core.h"

using namespace ngx::Core::Support;

int LockTest() {
    spin_lock Lock;

    Lock.lock();
    Lock.Unlock();

    return 0;
}
