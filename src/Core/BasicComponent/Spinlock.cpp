#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

void Spinlock::Lock() {
    while (LockAtomic.test_and_set()) {
        RelaxMachine();
    }
}

void Spinlock::Unlock() {
    LockAtomic.clear();
}

bool Spinlock::TryLock() {

    if (LockAtomic.test_and_set()) {
        RelaxMachine();
        return false;
    }

    return true;
}

BigSpinlock::BigSpinlock() : Spinlock() {}

void BigSpinlock::Lock() {

    while (LockAtomic.test_and_set()) {
        std::this_thread::yield();
    }
}

bool BigSpinlock::TryLock() {

    if (LockAtomic.test_and_set()) {
        std::this_thread::yield();
        return false;
    }

    return true;
}

Mutex::Mutex() :BackendMutex(){}

void Mutex::Lock() {
    BackendMutex.lock();
}

void Mutex::Unlock() {
    BackendMutex.unlock();
}

bool Mutex::TryLock() {
    return BackendMutex.try_lock();
}