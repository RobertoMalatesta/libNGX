#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

SocketEventDomain::SocketEventDomain(size_t PoolSize, int ThreadCount) :
        EventDomain(PoolSize, ThreadCount), Timers() {
}

SocketEventDomain::~SocketEventDomain() {
    Timers.~TimerTree();
}

RuntimeError SocketEventDomain::EventDomainProcess() {

    RuntimeError Error{0};

    Error = EventDomain::EventDomainProcess();

    if (Error.GetCode() == 0) {
        Timers.QueueExpiredTimer(&TPool, TIMER_PRCESS_COUNT);
    } else {
        return Error;
    }
    return {0};
}