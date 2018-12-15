#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

SocketTimerHub::~SocketTimerHub() {

    for (RBNode *It = this->Begin(); It ; It = Next(It)) {
        Erase(It);
    }
};

int SocketTimerHub::QueueExpiredTimer() {

    Timer *Temp;
    uint64_t Timestamp = GetHighResolutionTimestamp();

    for (RBNode *It = Begin(); It ; It = Next(It)) {

        Temp = (Timer *)It;
        Socket *S = Socket::TimerToSocket(Temp);

        if (S->TryLock()) {

            if (Temp->Timestamp > Timestamp) {

                S->Unlock();
                break;
            }

            S->PostPromise(Temp->Callback, Temp->Argument);

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

int SocketTimerHub::AttachTimer(Timer &T) {

    TimerHubLock.Lock();

    if (T.Interval > 0) {
        T.Timestamp = GetHighResolutionTimestamp() + T.Interval;
        Insert(&T);
    }

    TimerHubLock.Unlock();

    return 0;
}

int SocketTimerHub::DetachTimer(Timer &T) {

    TimerHubLock.Lock();

    if (T.IsTimerAttached()) {

        Erase(&T);
        T.Mode = TM_CLOSED, T.Interval = 0;
    }

    TimerHubLock.Unlock();

    return 0;
}