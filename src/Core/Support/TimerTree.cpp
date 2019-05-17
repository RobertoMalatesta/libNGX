#include "Core/Core.h"

using namespace ngx::Core::Support;

TimerTree::~TimerTree() {
    for (RBNode *it = this->begin(); it; it = next(it)) {
        remove(it);
    }
};

RuntimeError TimerTree::attachTimer(Timer &T) {

    TimerHubLock.lock();

    if (T.Interval > 0) {
        T.Timestamp = GetHighResolutionTimestamp() + T.Interval;
        insert(T);
    }

    TimerHubLock.Unlock();

    return {0};
}

RuntimeError TimerTree::detachTimer(Timer &T) {

    TimerHubLock.lock();

    if (T.IsTimerAttached()) {
        remove(&T);
        T.Mode = TM_CLOSED;
    }

    TimerHubLock.Unlock();

    return {0};
}

RuntimeError TimerTree::preemptTimer(Timer &T) {
    TimerHubLock.lock();

    if (T.IsTimerAttached()) {
        remove(&T);
        T.Timestamp = 0;
        insert(T);
    }

    TimerHubLock.Unlock();

    return {0};
}

Timer* TimerTree::begin() const{
    return static_cast<Timer *>(RBTree::begin());
}

Timer* TimerTree::next(Timer *T) const {
    if (T== nullptr) return nullptr;
    return static_cast<Timer *>(RBTree::next(T));
}

RuntimeError TimerTree::nextExpiredTimer(uint64_t Timestamp, Timer *&T) {

}

