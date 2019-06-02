#include "Core/Support/spin_lock.h"

using namespace ngx::Core::Support;

int LockTest() {
    spin_lock Lock;

    Lock.lock();
    Lock.unlock();

    return 0;
}
