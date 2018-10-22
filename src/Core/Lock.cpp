#include "Core/Core.h"

using namespace ngx::Core;

void SpinLock::Lock() {
    while (LockAtomic.test_and_set()) {
        RelaxMachine();
    }
}

void SpinLock::Unlock() {
    LockAtomic.clear();
}

BigSpinlock::BigSpinlock() : SpinLock() {}

void BigSpinlock::Lock() {
    while (LockAtomic.test_and_set()) {
        std::this_thread::yield();
    }
}