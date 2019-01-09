#include "HTTP/HTTP.h"

using namespace ngx::HTTP;

HTTPConnectionCollector::HTTPConnectionCollector(uint32_t CollectorSize) :
        BackendAllocator(),
        AllocatorBuild(&BackendAllocator),
        Collector(CollectorSize) {
}

HTTPConnectionCollector::~HTTPConnectionCollector() {
    Reset();
}

int HTTPConnectionCollector::Get(HTTPConnection *&C, int SocketFD, SocketAddress &Address) {

    Queue *Q;
    C = nullptr;

    if (!CollectorSentinel.IsEmpty()) {
        Q = CollectorSentinel.GetNext();
        Q->Detach();

        CollectorSize += 1;
        C = HTTPConnection::FromCollectQueue(Q);
    } else {
        if (Build(C) != 0) {
            return C = nullptr, -1;
        }
    }

    C->Unlock();
    C->SetSocketAddress(SocketFD, Address);
    return 0;
}

int HTTPConnectionCollector::Put(HTTPConnection *&Item) {

    if (CollectorSize <= 0) {
        Destroy(Item);
        Item = nullptr;
    } else {
        Item->Reset();
        CollectorSentinel.Append(&Item->ReuseItem);
        CollectorSize -= 1;
    }

    if (AllocateCount++ % (RECYCLE_GC_ROUND) == 0) {
        BackendAllocator.GC();
    }

    return 0;
}

void HTTPConnectionCollector::Reset() {

    HTTPConnection *Item;

    while (!CollectorSentinel.IsEmpty()) {

        Item = HTTPConnection::FromCollectQueue(CollectorSentinel.GetNext());
        Item->ReuseItem.Detach();
        Item->Reset();
        Destroy(Item);

        CollectorSize += 1;
    }
}