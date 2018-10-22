namespace ngx::Core {

    class TimerTreeNode : public RBTreeNode {

        friend class TimerTree;

    protected:
        uint64_t Timestamp = 0;
        PromiseCallback *Callback = nullptr;
        void *Argument = nullptr;

        RBTreeNode *GetLeft() { return this->Left; }

        RBTreeNode *GetRight() { return this->Right; }

        virtual int Compare(TimerTreeNode *Node);

    public:

        TimerTreeNode() {};

        TimerTreeNode(uint64_t Timestamp, PromiseCallback *Callback, void *Argument) {
            this->Timestamp = Timestamp;
            this->Callback = Callback;
            this->Argument = Argument;
        };

        ~TimerTreeNode() = default;

        static RBTreeNode *
        CreateFromAllocator(MemAllocator *Allocator, uint64_t MillSecond, PromiseCallback *Callback, void *Argument);

        static void FreeFromAllocator(MemAllocator *Allocator, RBTreeNode **Node);
    };

    class TimerTree : public RBTree {

    protected:
        TimerTree(MemAllocator *Allocator);

        ~TimerTree();

    public:
        static TimerTree *CreateFromAllocator(MemAllocator *ParentAllocator, MemAllocator *Allocator);

        static void FreeFromAllocator(MemAllocator *ParentAllocator, TimerTree **TheRBTree);

        int PostTimerPromise(uint64_t MillSecond, PromiseCallback function, void *args);

        int QueueExpiredTimer(ThreadPool *TPool);
    };
}