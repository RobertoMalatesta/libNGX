#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

SocketEventDomain::SocketEventDomain(size_t PoolSize, int ThreadCount) :
        EventDomain(PoolSize, ThreadCount), THub() {
}

RuntimeError SocketEventDomain::EventDomainProcess() {

    RuntimeError Error{0};

    Error = EventDomain::EventDomainProcess();

    if (Error.GetCode() == 0) {
        THub.QueueExpiredTimer(&TPool, TIMER_PROCESS_COUNT);
    } else {
        return Error;
    }
    return {0};
}