#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

HTTPConnectionRecycleBin::HTTPConnectionRecycleBin(uint64_t RecycleBinSize) :
        BackendAllocator(),
        AllocatorBuild(&BackendAllocator),
        RecycleBin(RecycleBinSize) {
}


int HTTPConnectionRecycleBin::Get(HTTPConnection *&C, int SocketFD, SocketAddress &Address) {

    Queue *Q;
    C = nullptr;

    if (!RecycleSentinel.IsEmpty()) {
        Q = RecycleSentinel.GetNext();
        Q->Detach();
        RecycleSize -= 1;
        C = HTTPConnection::FromRecycleQueue(Q);
    }
    else{
        if (Build(C) != 0) {
            return C = nullptr, -1;
        }
    }
    C->Unlock();
    C->SetSocketAddress(SocketFD, Address);
    return 0;
}

int HTTPConnectionRecycleBin::Put(HTTPConnection *&Item) {

    if (RecycleSize >= RecycleMaxSize) {
        Destroy(Item);
        Item = nullptr;
    } else {
        Item->Reset();
        RecycleSize += 1;
        RecycleSentinel.Append(&Item->RecycleItem);
    }

    if (AllocateCount++ % (RECYCLE_GC_ROUND) == 0) {
        BackendAllocator.GC();
    }

    return 0;
}

HTTPConnectionRecycleBin::~HTTPConnectionRecycleBin() {

    HTTPConnection *Item;

    while (!RecycleSentinel.IsEmpty()) {
        Item = HTTPConnection::FromRecycleQueue(RecycleSentinel.GetNext());
        RecycleSize -= 1;
        Item->RecycleItem.Detach();
        Item->~HTTPConnection();
        Destroy(Item);
    }
}