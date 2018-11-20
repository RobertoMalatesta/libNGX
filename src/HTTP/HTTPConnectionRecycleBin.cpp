#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

HTTPConnectionRecycleBin::HTTPConnectionRecycleBin(uint64_t RecycleBinSize) :
        BackendAllocator(),
        AllocatorBuild(&BackendAllocator),
        RecycleBin(RecycleBinSize) {}


int HTTPConnectionRecycleBin::Get(HTTPConnection *&C, int SocketFD, SocketAddress &TargetSocketAddress) {

    if (RecycleSentinel.IsEmpty() && Build(C) == -1) {
        return -1;
    } else {
        C = (HTTPConnection *) RecycleSentinel.GetHead();
        RecycleSize -= 1;
        C->Detach();
    }

    C->SetSocketAddress(SocketFD, TargetSocketAddress);
    return 0;
}

int HTTPConnectionRecycleBin::Put(HTTPConnection *&Item) {

    Item->Reset();

    if (RecycleSize >= RecycleMaxSize) {
        Destroy(Item);
    } else {
        RecycleSize += 1;
        RecycleSentinel.Append(Item);
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