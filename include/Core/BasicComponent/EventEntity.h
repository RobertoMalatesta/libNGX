static void DiscardPromise(void *, ThreadPool *) {
    printf("discard promise!");
}

class EventEntity {
protected:
    SpinLock Lock;
    uint32_t Event;
};

union UserArgument {
    void *Ptr;
    unsigned int UInt;
};

struct EventPromiseArgs {
    UserArgument UserArguments[8];
};
