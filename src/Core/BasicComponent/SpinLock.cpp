#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

SpinLock::SpinLock() {
    pthread_spin_init(&lock, 0);
}

SpinLock::~SpinLock() {
    pthread_spin_destroy(&lock);
}

void SpinLock::Lock() {
    pthread_spin_lock(&lock);
}

void SpinLock::Unlock() {
    pthread_spin_unlock(&lock);
}

volatile bool SpinLock::TryLock() {
    return pthread_spin_trylock(&lock) == 0;
}

Mutex::Mutex() : BackendMutex() {}

void Mutex::Lock() {
    BackendMutex.lock();
}

void Mutex::Unlock() {
    BackendMutex.unlock();
}

volatile bool Mutex::TryLock() {
    return BackendMutex.try_lock();
}