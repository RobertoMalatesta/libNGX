namespace ngx::Core {

    // Timer Entity

    class TimerTree : public RBTree{
    private:
        TimerTree();
        ~TimerTree();

    public:

        // Create from Allocator
        // Free from Allocator

        // PostTimerPromise( msec, function, void *args)
        // QueryTimerPromise(Threadpool TPool);

    };
}