class LockType {
public:
    virtual void Lock() = 0;
    virtual volatile bool TryLock() = 0;
    virtual void Unlock() = 0;
};