#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

HTTPConnectionRecycleBin::HTTPConnectionRecycleBin(uint64_t RecycleBinSize) :
        BackendAllocator(),
        AllocatorBuild(&BackendAllocator),
        RecycleBin(RecycleBinSize) {}


int HTTPConnectionRecycleBin::Get(HTTPConnection *&C, int SocketFD, SocketAddress &TargetSocketAddress) {

    if (!RecycleSentinel.IsEmpty()) {
        C = (HTTPConnection *) RecycleSentinel.GetHead();
        RecycleSize -= 1;
        C->Detach();
    }
    else{
        C = new HTTPConnection();
    }

    C->SetSocketAddress(SocketFD, TargetSocketAddress);
    return 0;
}

int HTTPConnectionRecycleBin::Put(HTTPConnection *&Item) {


    if (RecycleSize >= RecycleMaxSize) {
        delete Item;
        Item = nullptr;
    } else {
        Item->Reset();
        RecycleSize += 1;
        RecycleSentinel.Append(Item);
    }

    if (AllocateCount++ % (RECYCLE_GC_ROUND) == 0) {
        BackendAllocator.GC();
    }

    return 0;
}

HTTPConnectionRecycleBin::~HTTPConnectionRecycleBin() {

    HTTPConnection *Item;

    while (!RecycleSentinel.IsEmpty()) {
        Item = (HTTPConnection *) RecycleSentinel.GetHead();
        RecycleSize -= 1;
        Item->Detach();
        Destroy(Item);
    }
}