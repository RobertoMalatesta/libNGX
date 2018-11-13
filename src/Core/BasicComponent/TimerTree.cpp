#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

int Timer::Compare(Timer *Node) {
    if (this->Timestamp == Node->Timestamp) {
        return 0;
    } else if (this->Timestamp > Node->Timestamp) {
        return 1;
    } else {
        return -1;
    }
}

TimerTree::TimerTree(MemAllocator *Allocator) : RBTree(), AllocatorBuild(Allocator) {

    Timer *TempTimer;

    if (Build(TempTimer) == 0) {
        Root = Sentinel = TempTimer;
    }
}

TimerTree::~TimerTree() {

    RBTreeNode *Temp;
    Timer *TempTimer;

    for (RBTreeNode *It = Minimum(); It != nullptr && It != Sentinel;) {
        Temp = It, TempTimer = (Timer *) It;
        It = Next(It);
        Delete(Temp), Destroy(TempTimer);
    }

    TempTimer = (Timer *) (Sentinel);
    Destroy(TempTimer);

    RBTree::~RBTree();
};

int TimerTree::QueueExpiredTimer(ThreadPool *TPool) {

    RBTreeNode *It;
    Timer *Temp;

    SpinlockGuard LockGuard(&Lock);

    uint64_t Timestamp = GetTimeStamp();

    for (It = Minimum(); It != Sentinel && It != nullptr;) {

        Temp = (Timer *) It;
        It = Next(It);

        if (Temp->Timestamp > Timestamp) {
            break;
        }

        TPool->PostPromise(Temp->Callback, Temp->Argument);

        Delete(Temp);
    }
    return 0;
}

int TimerTree::AttachTimer(Timer &T) {
    SpinlockGuard LockGuard(&Lock);
    Insert(&T);
    return 0;
}
