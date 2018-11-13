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

TimerTree::TimerTree(MemAllocator *Allocator) : RBTree() {

    void *PointerToSentinel = Allocator->Allocate(sizeof(Timer));

    if (nullptr == PointerToSentinel) {
        return;
    }

    Root = Sentinel = new(PointerToSentinel) Timer();
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

int TimerTree::PostTimerPromise(uint64_t Seconds, PromiseCallback *Callback, void *Argument) {

    uint64_t TimeStamp = GetTimeStamp() + Seconds;

    Timer *Timer;

    if (!Build(Timer)) {
        return -1;
    }

    Timer->Timestamp = TimeStamp;
    Timer->Callback = Callback;
    Timer->Argument = Argument;
    Insert(Timer);

    return 0;
}

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
    Delete(&T);
    return 0;
}

