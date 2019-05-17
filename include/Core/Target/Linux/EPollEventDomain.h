//===- EPollEventDomain.h - Implement epoll based event domain -----*- C++ -*-===//
//
//                     The NGX Server Infrastructure
//
// This file is distributed under the MIT Open Source License. See LICENSE.TXT
// for detail.
//
//===-------------------------------------------------------------------------===//
//
//  This file implement epoll based socket file EventDomain.
//
//===-------------------------------------------------------------------------===//

using namespace ngx::Core::Support;

namespace Config {

    const int EPOLL_EVENT_BATCH_SIZE = 128;
    const int EPOLL_EVENT_WAIT_TIME = 1;    // ms
    const int EPOLL_EVENT_MAX_CONNECTION = 32768;
}

class EPollEventDomain : public SocketEventDomain {
protected:
    int EPollFD = -1;
    spin_lock EPollLock;
public:

    EPollEventDomain(int ThreadCount, int EPollSize);

    ~EPollEventDomain();

    virtual EventError AttachSocket(Socket &S, EventType Type);

    virtual EventError DetachSocket(Socket &S, EventType Type);

    virtual RuntimeError EventLoop();
};
