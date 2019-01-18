//===------------------ EventDomain.h - represent a Connection ------*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
// Basic event processing unit
//
//===-------------------------------------------------------------------------===//

class EventDomain {
protected:
    ThreadPool TPool;
    SpinLock TPoolLock;
public:
    EventDomain(int ThreadCount);

    ~EventDomain() {
        LockGuard LockGuard(&TPoolLock);
        TPool.~ThreadPool();
    }

    RuntimeError BindEventThread(EventEntity &Entity);

    RuntimeError UnbindEventThread(EventEntity &Entity);

    virtual RuntimeError EventLoop();
};
