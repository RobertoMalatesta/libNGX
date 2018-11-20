#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

int Timer::Compare(Timer *Node) {

    if (Timestamp > Node->Timestamp) {
        return 1;
    } else if (Timestamp < Node->Timestamp) {
        return -1;
    }
    return 0;
}

TimerTree::TimerTree(MemAllocator *Allocator) : RBTree(), AllocatorBuild(Allocator) {

    Timer *TempTimer;

    if (Build(TempTimer) == 0) {
        Root = Sentinel = TempTimer;
    }
}

TimerTree::~TimerTree() {

    Timer *Temp;

    for (RBTreeNode *It = Minimum(); It != Sentinel && It != nullptr;) {
        Temp = (Timer *) It;
        It = Next(It);
        Delete(Temp);
    }

    if (Sentinel) {
        Temp = (Timer *) Sentinel;
        Destroy(Temp);
        Root = Sentinel = nullptr;
    }

    RBTree::~RBTree();
};

int TimerTree::QueueExpiredTimer(ThreadPool *TPool) {

    Timer *Temp;
    uint64_t Timestamp = GetHighResolutionTimestamp();

    for (RBTreeNode *It = Minimum(); It != Sentinel && It != nullptr;) {

        Temp = (Timer *) It;
        It = Next(It);

        if (Temp->Timestamp > Timestamp) {
            break;
        }

        TPool->PostPromise(Temp->Callback, Temp->Argument);
        Temp->Lock();
        Delete(Temp);
        Temp->Unlock();

        if (Temp->Mode == TM_INTERVAL && Temp->Interval > 0) {
            Temp->Timestamp = Timestamp + Temp->Interval;
            Insert(Temp);
        } else {
            Temp->Mode = TM_CLOSED;
        }
    }
    return 0;
}

int TimerTree::AttachTimer(Timer &T) {

    if (T.Interval > 0) {
        T.Timestamp = GetHighResolutionTimestamp() + T.Interval;
        T.Lock();
        Insert(&T);
        T.Unlock();
    }
    return 0;
}

int TimerTree::DetachTimer(Timer &T) {

    if (T.IsTimerAttached()) {
        T.Lock();
        Delete(&T);
        T.Mode = TM_CLOSED, T.Interval = 0;
        T.Unlock();
    }

    return 0;
}