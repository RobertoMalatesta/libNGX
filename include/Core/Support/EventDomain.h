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
    spin_lock Lock;
    ThreadPool Pool;
    TimerTree Tree;

    void EmitExpiredTimer();

public:
    EventDomain(int ThreadCount);

    ~EventDomain() {
        std::lock_guard<spin_lock> g(Lock);
        Pool.~ThreadPool();
    }

    RuntimeError BindEventThread(EventEntity &Entity);

    RuntimeError UnbindEventThread(EventEntity &Entity);

    virtual RuntimeError EventLoop();
    virtual EventError AttachListen(Listen &L) = 0;
    virtual EventError DetachListen(Listen &L) = 0;
    virtual EventError AttachConnection(Connection &C) = 0;
    virtual EventError DetachConnection(Connection &C) = 0;
};
