typedef uint32_t EventType;

static void DiscardPromise(void *, ThreadPool *) {
    printf("discard promise!");
}

class EventEntity {
protected:
    EventType Event = 0;
    EventType AttachedEvent = 0;
    Thread *BackendWorker = nullptr;
    friend class EventDomain;

public:
    RuntimeError PostPromise(PromiseCallback *Callback, void *Argument);
};
