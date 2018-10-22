#include "Http/Http.h"

using namespace ngx::Http;

HttpConnectionRecycler::HttpConnectionRecycler(size_t BlockSize, uint64_t BufferRecyclerSize, uint64_t RecyclerSize) :
        Recycler(RecyclerSize),
        BB(BlockSize, BufferRecyclerSize) {}

HttpConnection *HttpConnectionRecycler::Get(int SocketFD, SocketAddress &SocketAddress) {
    HttpConnection *Ret;
    SpinlockGuard LockGuard(&Lock);
    if (RecycleSentinel.IsEmpty()) {
        Ret = new HttpConnection(SocketFD, SocketAddress, BB);
    } else {
        Ret = (HttpConnection *) RecycleSentinel.GetHead();
        RecycleSize -= 1;
        Ret->Detach();
        Ret->SocketFd = SocketFD;
        Ret->SocketAddress = SocketAddress;
    }
    return Ret;
}

void HttpConnectionRecycler::Put(HttpConnection *Item) {

    SpinlockGuard LockGuard(&Lock);

    Item->Reset();

    if (RecycleSize >= RecycleMaxSize) {
        delete Item;
    } else {
        RecycleSize += 1;
        RecycleSentinel.Append(Item);
    }
}