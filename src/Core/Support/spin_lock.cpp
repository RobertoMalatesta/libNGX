#include "Core/Core.h"

using namespace ngx::Core::Support;

spin_lock::spin_lock() {
    pthread_spin_init(&l, 0);
}

spin_lock::~spin_lock() {
    pthread_spin_destroy(&l);
}

void spin_lock::Lock() {
    pthread_spin_lock(&l);
}

void spin_lock::Unlock() {
    pthread_spin_unlock(&l);
}

volatile bool spin_lock::TryLock() {
    return pthread_spin_trylock(&l) == 0;
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