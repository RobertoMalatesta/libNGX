#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

SocketEventDomain::SocketEventDomain(size_t PoolSize, int ThreadCount) :
        EventDomain(PoolSize, ThreadCount), Timers(&Allocator) {
}

SocketEventDomain::~SocketEventDomain() {
    Timers.~TimerTree();
}

RuntimeError SocketEventDomain::EventDomainProcess() {

    RuntimeError Error{0};

    Error = EventDomain::EventDomainProcess();

    if (Error.GetCode() == 0) {
        Timers.QueueExpiredTimer(&TPool);
        return {0};
    } else {
        return Error;
    }
}