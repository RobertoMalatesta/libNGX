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
#include "Core/Support/Error.h"
#include "Core/Support/Listen.h"
#include "Core/Support/Connection.h"
#include "Core/Support/EventDomain.h"

namespace ngx {
namespace Core {
namespace Target {
namespace Linux {

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

    EventError attachListen(Listen &L) override;
    EventError detachListen(Listen &L) override;
    EventError attachConnection(Connection &C) override;
    EventError detachConnection(Connection &C) override;
    RuntimeError eventLoop() override ;
};

} // Linux
} // Target
} // Core
} // ngx

