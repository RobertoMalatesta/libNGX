class Spinlock : public LockType {
protected:
    pthread_spinlock_t lock;
public:
    Spinlock();

    ~Spinlock();

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

