#include "Core/Core.h"

using namespace ngx::Core::Support;

TimerTree::~TimerTree() {
    for (Timer *it = this->begin(); it; it = next(it)) {
        remove(it);
    }
};

RuntimeError TimerTree::attach(Timer &T) {

    if (T.isAttached())
        return {EALREADY, "timer already attched"};
    if (T.Interval == 0)
        return {EBADF, "timer not set or bad interval"};

    T.Timestamp = GetHighResolutionTimestamp() + T.Interval;
    insert(T);
    return {0};
}

RuntimeError TimerTree::detach(Timer &T) {
    if (T.isAttached()) {
        remove(&T);
    }
    return {0};
}

RuntimeError TimerTree::setOnce(Timer &T, uint64_t Peroid, Job &J) {
    Timer Target{0, TM_ONCE, Peroid, J};
    T.Interval = Peroid, T.Mode = TM_ONCE;

    if (T.isAttached())
        return {EALREADY, "timer already attached"};

    return attach(T = Target);
}

RuntimeError TimerTree::setInterval(Timer &T, uint64_t Interval, Job &J) {
    Timer Target{0, TM_INTERVAL, Interval, J};

    if (T.isAttached())
        return {EALREADY, "timer already attached"};

    return attach(T = Target);
}

RuntimeError TimerTree::respawnInterval(Timer &T) {
    if (!T.isInterval())
        return {EBADE, "not a interval timer"};
    else if (T.Interval == 0)
        return {EBADE, "interval equals to 0, will no nothing"};

    detach(T);
    T.Timestamp = GetHighResolutionTimestamp() + T.Interval;
    return attach(T);
}

RuntimeError TimerTree::preempt(Timer &T) {
    if (!T.isAttached())
        return {EALREADY, "timer is not attached"};

    detach(T);
    T.Timestamp = GetHighResolutionTimestamp();
    return attach(T);
}

RuntimeError TimerTree::stop(Timer &T) {
    if (T.isAttached()) {
        remove(&T);
        return {0};
    }
    return {EALREADY, "timer is not attached"};
}

Timer *TimerTree::begin() const {
    return static_cast<Timer *>(RBTree::begin());
}

Timer *TimerTree::next(Timer *T) const {
    if (T == nullptr) return nullptr;
    return static_cast<Timer *>(RBTree::next(T));
}

