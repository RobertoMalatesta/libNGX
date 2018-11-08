#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

HTTPConnectionRecycler::HTTPConnectionRecycler(uint64_t RecyclerSize) :
        Recycler(RecyclerSize){}

HTTPConnection *HTTPConnectionRecycler::Get(int SocketFD, SocketAddress &TargetSocketAddress) {
    HTTPConnection *Ret;

    if (RecycleSentinel.IsEmpty()) {
        Ret = new HTTPConnection(SocketFD, TargetSocketAddress);
    } else {
        Ret = (HTTPConnection *) RecycleSentinel.GetHead();
        RecycleSize -= 1;
        Ret->Detach();
        Ret->SetSocketAddress(SocketFD, TargetSocketAddress);
    }
    return Ret;
}

void HTTPConnectionRecycler::Put(HTTPConnection *Item) {

    Item->Reset();

    if (RecycleSize >= RecycleMaxSize) {
        delete Item;
    } else {
        RecycleSize += 1;
        RecycleSentinel.Append(Item);
    }
}