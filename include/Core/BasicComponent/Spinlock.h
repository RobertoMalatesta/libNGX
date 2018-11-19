class Spinlock : public LockType {
protected:
    std::atomic_flag LockAtomic = ATOMIC_FLAG_INIT;
public:
    Spinlock() {
        LockAtomic.clear();
    }

    virtual void Lock();

    virtual void Unlock();

    virtual bool TryLock();
};

class BigSpinlock : public Spinlock {
public:
    BigSpinlock();

    virtual void Lock();

    virtual bool TryLock();
};


class Mutex : public LockType{
private:
    std::mutex BackendMutex;
public:
    Mutex();
    virtual void Lock();
    virtual void Unlock();
    virtual bool TryLock();
};

class LockGuard {

protected:
    LockType *Lock;
public:
    LockGuard(LockType *Lock) {
        this->Lock = Lock;
        Lock->Lock();
    }

    ~LockGuard() {
        Lock->Unlock();
    }
};

