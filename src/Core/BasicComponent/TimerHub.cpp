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

TimerHub::TimerHub() : RBTree() {
    Root = Sentinel = &_Sentinet;
}

TimerHub::~TimerHub() {

    Timer *Temp;

    for (RBTreeNode *It = Minimum(); It != Sentinel && It != nullptr;) {
        Temp = (Timer *) It;
        It = Next(It);
        Delete(Temp);
    }

    Root = Sentinel = nullptr;

    RBTree::~RBTree();
};

int TimerHub::QueueExpiredTimer(ThreadPool *TPool, uint32_t Count) {

    Timer *Temp;
    uint64_t Timestamp = GetHighResolutionTimestamp();

    for (RBTreeNode *It = Minimum(); It != Sentinel && It != nullptr && Count >= 0; Count--) {

        Temp = (Timer *)It;
        Socket *S = Socket::TimerToSocket(Temp);
        It = Next(It);

        if (S->TryLock()) {
            if (Temp->Timestamp > Timestamp) {
                S->Unlock();
                break;
            }

            TPool->PostPromise(Temp->Callback, Temp->Argument);

            TimerHubLock.Lock();
            Delete(Temp);

            if (Temp->Mode == TM_INTERVAL && Temp->Interval > 0) {
                Temp->Timestamp = Timestamp + Temp->Interval;
                Insert(Temp);
            } else {
                Temp->Mode = TM_CLOSED;
            }

            TimerHubLock.Unlock();
            S->Unlock();
        }
    }
    return 0;
}

int TimerHub::AttachTimer(Timer &T) {

    TimerHubLock.Lock();

    if (T.Interval > 0) {
        T.Timestamp = GetHighResolutionTimestamp() + T.Interval;
        Insert(&T);
    }

    TimerHubLock.Unlock();

    return 0;
}

int TimerHub::DetachTimer(Timer &T) {

    TimerHubLock.Lock();

    if (T.IsTimerAttached()) {
        Delete(&T);
        T.Mode = TM_CLOSED, T.Interval = 0;
    }

    TimerHubLock.Unlock();

    return 0;
}