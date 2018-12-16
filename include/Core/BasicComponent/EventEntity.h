typedef uint32_t EventType;

class EventEntity {
protected:
    EventType Event = 0;
    EventType AttachedEvent = 0;
    Thread *BackendWorker = nullptr;
    friend class EventDomain;

public:
    RuntimeError PostJob(Job &J);
};
