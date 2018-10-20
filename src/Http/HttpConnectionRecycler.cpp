#include "Http/Http.h"

using namespace ngx::Http;

HttpConnectionRecycler::HttpConnectionRecycler(uint64_t RecyclerSize) : Recycler(RecyclerSize){}

HttpConnection* HttpConnectionRecycler::Get(int SocketFD, SocketAddress &SocketAddress,
                                            socklen_t SocketLength, Buffer &Buf) {
    HttpConnection *Ret;
    SpinlockGuard LockGuard(&Lock);

    if (RecycleSentinel.IsEmpty()) {
        Ret = new HttpConnection(SocketFD, SocketAddress, SocketLength, Buf);
    }
    else {
        Ret = (HttpConnection *)RecycleSentinel.GetHead();
        Ret->Detach();
        Ret->SocketFd = SocketFD;
        Ret->SocketAddress = SocketAddress;
        Ret->SocketLength = SocketLength;
    }
    return  Ret;
}

void HttpConnectionRecycler::Put(HttpConnection *Item) {

    SpinlockGuard LockGuard(&Lock);

    if (RecycleSize >= RecycleMaxSize) {
        delete Item;
    }
    else {
        RecycleSize += 1;
        Item->Reset();
        RecycleSentinel.Append(Item);
    }
}