typedef uint32_t EventType;

static void DiscardPromise(void *, ThreadPool *) {
    printf("discard promise!");
}

class EventEntity {
protected:
    SpinLock Lock;
    EventType Event;
    EventType AttachedEvent;
};
