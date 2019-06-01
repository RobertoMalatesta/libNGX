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
#include "Core/Support/TimerTree.h"
#include "Core/Support/ThreadPool.h"

#ifndef LIBNGX_CORE_SUPPORT_EVENTDOMAIN
#define LIBNGX_CORE_SUPPORT_EVENTDOMAIN
namespace ngx {
namespace Core {
namespace Support {

enum Event {
    READ    = 1U << 0,
    WRITE   = 1U << 1,
    TIMER   = 1U << 2,
    CLOSED  = 1U << 3,
    USR1    = 1U << 4,
    USR2    = 1U << 5,
};
typedef uint32_t Event_t;

class Listen;
class Connection;
class EventDomain;
class EventSubscriber {
protected:
    Thread *Worker;
    ThreadFn *Fn = nullptr;

public:
    spin_lock Lock;
    RuntimeError postEvent(Event_t E) {
        if (Fn == nullptr)
            return {EINVAL, "event process routine is not set"};
        else if (Worker == nullptr)
            return {EINVAL, "worker is is not specified"};

        Job J{Fn, reinterpret_cast<void *>(this), reinterpret_cast<void *>(E)};

        return Worker->postJob(J);
    }
    virtual RuntimeError bindDomain(EventDomain &D) = 0;
    virtual RuntimeError unbindDomain() = 0;
};

class EventDomain {
protected:
    spin_lock Lock;
    ThreadPool Pool;
    TimerTree Tree;
    void emitExpiredTimer();

public:
    explicit EventDomain(int ThreadCount);
    ~EventDomain() {
        std::lock_guard<spin_lock> g(Lock);
        Pool.~ThreadPool();
    }

    virtual RuntimeError eventLoop();
    virtual EventError attachListen(Listen &L) = 0;
    virtual EventError detachListen(Listen &L) = 0;
    virtual EventError attachConnection(Connection &C) = 0;
    virtual EventError detachConnection(Connection &C) = 0;
    inline RuntimeError setTimerOnce(Timer &T, uint64_t Interval, void *pData) {
        std::lock_guard<spin_lock> g(Lock);
        return Tree.setOnce(T, Interval, pData);
    }
    inline RuntimeError setTimeInterval(Timer &T, uint64_t Interval, void *pData) {
        std::lock_guard<spin_lock> g(Lock);
        return Tree.setInterval(T, Interval, pData);
    }
    inline RuntimeError stopTimer(Timer &T) {
        std::lock_guard<spin_lock> g(Lock);
        return Tree.stop(T);
    }
};

} // Support
} // Core
} // ngx
#endif
