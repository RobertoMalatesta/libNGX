#include <mutex>

#ifndef LIBNGX_CORE_SUPPORT_SPINLOCK
#define LIBNGX_CORE_SUPPORT_SPINLOCK
namespace ngx {
namespace Core {
namespace Support {

class spin_lock {
protected:
    pthread_spinlock_t l;

public:
    spin_lock();
    ~spin_lock();
    virtual void Lock();
    virtual void Unlock();
    virtual void lock() { Lock(); };
    virtual void unlock() { Unlock(); };
    virtual volatile bool TryLock();
};

class Mutex {
private:
    std::mutex BackendMutex;

public:
    Mutex();
    virtual void Lock();
    virtual void Unlock();
    virtual volatile bool TryLock();
};

} // Support
} // Core
} // ngx

#endif