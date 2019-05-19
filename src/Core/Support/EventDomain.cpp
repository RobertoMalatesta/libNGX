#include "Core/Core.h"

using namespace ngx::Core::Support;

EventDomain::EventDomain(int numThreads)
        : Lock(), Pool(numThreads), Tree() {
}

RuntimeError EventDomain::eventLoop() {
    std::lock_guard<spin_lock> g(Lock);
    emitExpiredTimer();
    return {0};
}

void EventDomain::emitExpiredTimer() {

    uint64_t Timestamp = GetHighResolutionTimestamp();

    for (Timer *next, *it = Tree.begin(); it;) {
        if (it->getTimestamp() > Timestamp) {
            break;
        }

        next = Tree.next(it);
        auto *Sub = static_cast<EventSubscriber *>(it->getData());

        if (Sub->Lock.TryLock()) {
            Tree.stop(*it);
            Sub->postEvent(Event::TIMER);
            if (it->isInterval()) {
                Tree.respawnInterval(*it);
            }
            Sub->Lock.Unlock();
        }
        it = next;
    }
}