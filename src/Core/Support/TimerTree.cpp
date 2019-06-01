#include "Core/Support/Time.h"
#include "Core/Support/Error.h"
#include "Core/Support/TimerTree.h"
#include <cerrno>

using namespace std;
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

RuntimeError TimerTree::setOnce(Timer &T, uint64_t Peroid, void *pData) {

    T.pData = pData;
    T.Mode = TM_ONCE;
    T.Interval = Peroid;

    if (T.isAttached())
        return {EALREADY, "timer already attached"};

    return attach(T);
}

RuntimeError TimerTree::setInterval(Timer &T, uint64_t Interval, void *pData) {

    T.pData = pData;
    T.Mode = TM_INTERVAL;
    T.Interval = Interval;

    if (T.isAttached())
        return {EALREADY, "timer already attached"};

    return attach(T);
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

