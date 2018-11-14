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
    Insert(&T);
    return 0;
}

int TimerTree::DetachTimer(Timer &T) {
    if (T.IsTimerAttached()) {
        Delete(&T);
    }
    return 0;
}