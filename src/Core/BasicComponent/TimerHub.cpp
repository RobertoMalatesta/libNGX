#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

TimerHub::~TimerHub() {

    for (RBNode *It = this->Begin(); It ; It = Next(It)) {
        Erase(It);
    }
};

int TimerHub::QueueExpiredTimer(ThreadPool *TPool, uint32_t Count) {

    Timer *Temp;
    uint64_t Timestamp = GetHighResolutionTimestamp();

    for (RBNode *It = Begin(); It && Count-- >0 ; It = Next(It)) {

        Temp = (Timer *)It;
        Socket *S = Socket::TimerToSocket(Temp);

        if (S->TryLock()) {
            if (Temp->Timestamp > Timestamp) {
                S->Unlock();
                break;
            }

            TPool->PostPromise(Temp->Callback, Temp->Argument);

            TimerHubLock.Lock();
            Erase(It);

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
        Erase(&T);
        T.Mode = TM_CLOSED, T.Interval = 0;
    }

    TimerHubLock.Unlock();

    return 0;
}