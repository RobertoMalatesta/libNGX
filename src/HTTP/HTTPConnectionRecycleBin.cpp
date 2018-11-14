#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

HTTPConnectionRecycleBin::HTTPConnectionRecycleBin(uint64_t RecycleBinSize) :
        RecycleBin(RecycleBinSize) {}


int HTTPConnectionRecycleBin::Get(HTTPConnection *&C, int SocketFD, SocketAddress &TargetSocketAddress) {

    if (RecycleSentinel.IsEmpty()) {
        C = new HTTPConnection(SocketFD, TargetSocketAddress);
    } else {
        C = (HTTPConnection *) RecycleSentinel.GetHead();
        RecycleSize -= 1;
        C->Detach();
        C->SetSocketAddress(SocketFD, TargetSocketAddress);
    }
    return 0;
}

int HTTPConnectionRecycleBin::Put(HTTPConnection *&Item) {

    Item->Reset();

    if (RecycleSize >= RecycleMaxSize) {
        delete Item;
    } else {
        RecycleSize += 1;
        RecycleSentinel.Append(Item);
    }
    return 0;
}