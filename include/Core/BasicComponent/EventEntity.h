typedef uint32_t EventType;

static void DiscardPromise(void *, ThreadPool *) {
    printf("discard promise!");
}

class EventEntity {
protected:
    Spinlock Lock;
    EventType Event = 0;
    EventType AttachedEvent = 0;
    Timer TimerNode;
    friend class EventDomain;
};
