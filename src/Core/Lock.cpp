#include "Core/Core.h"

using namespace ngx::Core;

void SpinLock::Lock() {
    while (LockAtomic.test_and_set()) {
        RelaxMachine();
    }
    return;
}

void SpinLock::Unlock() {
    LockAtomic.clear();
}
