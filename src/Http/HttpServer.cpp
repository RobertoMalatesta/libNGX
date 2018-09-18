#include "Http/Http.h"

using namespace ngx::Core;
using namespace ngx::Http;

HttpServer::HttpServer(
        size_t PoolSize,
        int ThreadCount,
        int EPollSize,
        int ConnectionRecycleSize,
        int BufferRecycleSize) :Domain(PoolSize, ThreadCount, EPollSize, &(HttpEventProcessPromise)) {
}

void HttpServer::HttpEventProcessPromise(void *Args, ThreadPool *TPool) {

    EPollEventProcessArguments *EPollArguments;

    if (nullptr == Args) {
        return;
    }

    EPollArguments = static_cast<EPollEventProcessArguments *>(Args);
    EPollEventDomain *Domain = EPollArguments->EventDomain;
    epoll_event *Events = EPollArguments->Events;
    Listening *Listening = EPollArguments->Listening;

    if (Domain == nullptr || Events == nullptr) {
        return;
    }

    Domain->FreeAllocatedMemory((void **)&EPollArguments);
    Domain->FreeAllocatedMemory((void **)&Events);
    Domain->EPollEnqueueListening(Listening);
}