#include "Core/Core.h"

using namespace ngx::Core::Support;

TimerTree::~TimerTree() {
    for (Timer *it = this->begin(); it; it = next(it)) {
        remove(it);
    }
};

RuntimeError TimerTree::attach(Timer &T) {
    if (T.Interval == 0 || T.Mode == TM_CLOSED)
        return {EBADF, "timer not set or bad interval"};

    T.Timestamp = GetHighResolutionTimestamp() + T.Interval;
    insert(T);
    return {0};
}

RuntimeError TimerTree::detach(Timer &T) {
    if (T.isAttached()) {
        remove(&T);
        T.Mode = TM_CLOSED;
    }
    return {0};
}

RuntimeError TimerTree::setOnce(Timer &T, uint64_t Peroid, Job &J) {
    Timer Target{0, TM_ONCE, Peroid, J};

    if (T.isAttached())
        return {EALREADY, "timer already attached"};

    return attach(T=Target);
}

RuntimeError TimerTree::setInterval(Timer &T, uint64_t Interval, Job &J) {
    Timer Target{0, TM_INTERVAL, Interval, J};

    if (T.isAttached())
        return {EALREADY, "timer already attached"};

    return attach(T=Target);
}

RuntimeError TimerTree::preemptTimer(Timer &T) {
    if (!T.isAttached())
        return {EALREADY, "timer is not attached"};

    remove(&T);
    T.Timestamp = GetHighResolutionTimestamp();
    insert(T);
    return {0};
}

RuntimeError TimerTree::resetTimer(Timer &T) {
    if (T.isAttached()) {
        remove(&T);
        return {0};
    }
    return {EALREADY, "timer is not attached"};
}

Timer* TimerTree::begin() const{
    return static_cast<Timer *>(RBTree::begin());
}

Timer* TimerTree::next(Timer *T) const {
    if (T== nullptr) return nullptr;
    return static_cast<Timer *>(RBTree::next(T));
}

