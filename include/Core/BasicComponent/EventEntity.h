static void DiscardPromise(void *, ThreadPool *) {
    printf("discard promise!");
}

class EventEntity {
protected:
    SpinLock Lock;
    uint32_t Event;
};
