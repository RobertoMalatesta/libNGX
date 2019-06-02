#include <mutex>
#include <pthread.h>
#ifndef LIBNGX_CORE_SUPPORT_SPINLOCK
#define LIBNGX_CORE_SUPPORT_SPINLOCK
namespace ngx {
namespace Core {
namespace Support {

class spin_lock {
protected:
    pthread_spinlock_t l;

public:
    spin_lock() { pthread_spin_init(&l, 0); }
    ~spin_lock() { pthread_spin_destroy(&l); };
    virtual void lock() { pthread_spin_lock(&l); };
    virtual void unlock() { pthread_spin_unlock(&l); };
    virtual volatile bool tryLock() { return pthread_spin_trylock(&l) == 0; };
};

} // Support
} // Core
} // ngx
#endif