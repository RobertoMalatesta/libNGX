class SpinLock : public LockType {
protected:
    pthread_rwlock_t lock;
public:
    SpinLock();

    ~SpinLock();

    virtual void Lock();

    virtual void Unlock();

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
