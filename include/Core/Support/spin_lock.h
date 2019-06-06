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
    void lock() { pthread_spin_lock(&l); };
    void unlock() { pthread_spin_unlock(&l); };
    volatile bool tryLock() { return pthread_spin_trylock(&l) == 0; };
};
class rw_lock {
protected:
    pthread_rwlock_t l;

public:
    rw_lock() { pthread_rwlock_init(&l, 0); }
    ~rw_lock() { pthread_rwlock_destroy(&l); };
    void rlock() { pthread_rwlock_rdlock(&l); };
    void wlock() { pthread_rwlock_wrlock(&l); };
    bool try_rlock() { return 0==pthread_rwlock_tryrdlock(&l); };
    bool try_wlock() { return 0==pthread_rwlock_trywrlock(&l); };
    void unlock() { pthread_rwlock_unlock(&l); };
};

} // Support
} // Core
} // ngx
#endif