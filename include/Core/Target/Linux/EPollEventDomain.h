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

class EPollEventDomain : public EventDomain {
protected:
    int EPollFD = -1;

    EventError attachFD(int FD, uint32_t E, void *pData);

    EventError detachFD(int FD);

public:
    EPollEventDomain(int numThreads, int epollSize);

    ~EPollEventDomain();

    virtual EventError attachListen(Listen &L) override;

    virtual EventError detachListen(Listen &L) override;

    virtual EventError attachConnection(Connection &C) override;

    virtual EventError detachConnection(Connection &C) override;

    virtual RuntimeError eventLoop() override ;
};
