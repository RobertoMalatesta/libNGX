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


namespace ngx {
    namespace Core {
        namespace Support {
            enum Event {
                READ = 1 << 0,
                WRITE = 1 << 1,
                TIMER = 1 << 2,
                CLOSED = 1 << 3,
                USR1 = 1 << 4,
                USR2 = 1 << 5,
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

                virtual RuntimeError Bind(EventDomain &D) { return {0}; };

                virtual RuntimeError UnBindBind(EventDomain &D) { return {0}; };

                RuntimeError postEvent(Event_t E) {
                    if (Fn == nullptr)
                        return {EINVAL, "event process routine is not set"};
                    else if (Worker == nullptr)
                        return {EINVAL, "worker is is not specified"};

                    Job J{Fn, reinterpret_cast<void *>(this), reinterpret_cast<void *>(E)};

                    return Worker->postJob(J);
                }
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
            };
        } // Support
    } // Core
} // ngx

