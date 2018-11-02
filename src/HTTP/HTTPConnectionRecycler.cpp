#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

HTTPConnectionRecycler::HTTPConnectionRecycler(size_t BlockSize, uint64_t BufferRecyclerSize, uint64_t RecyclerSize) :
        Recycler(RecyclerSize),
        BB(BlockSize, BufferRecyclerSize) {}

HTTPConnection *HTTPConnectionRecycler::Get(int SocketFD, SocketAddress &SocketAddress) {
    HTTPConnection *Ret;
    SpinlockGuard LockGuard(&Lock);
    if (RecycleSentinel.IsEmpty()) {
        Ret = new HTTPConnection(SocketFD, SocketAddress, BB);
    } else {
        Ret = (HTTPConnection *) RecycleSentinel.GetHead();
        RecycleSize -= 1;
        Ret->Detach();
        Ret->SocketFd = SocketFD;
        Ret->SocketAddress = SocketAddress;
    }
    return Ret;
}

void HTTPConnectionRecycler::Put(HTTPConnection *Item) {

    SpinlockGuard LockGuard(&Lock);

    Item->Reset();

    if (RecycleSize >= RecycleMaxSize) {
        delete Item;
    } else {
        RecycleSize += 1;
        RecycleSentinel.Append(Item);
    }
}