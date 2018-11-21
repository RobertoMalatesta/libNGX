#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

Spinlock::Spinlock() {
    pthread_spin_init(&lock, 0);
}

Spinlock::~Spinlock() {
    pthread_spin_destroy(&lock);
}

void Spinlock::Lock() {
    pthread_spin_lock(&lock);
}

void Spinlock::Unlock() {
    pthread_spin_unlock(&lock);
}

bool Spinlock::TryLock() {
    return pthread_spin_trylock(&lock) == 0;
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