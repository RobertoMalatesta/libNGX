namespace ngx::Core {

    class TimerTreeNode;
    class TimerTree;

    class TimerTreeNode : public RBTreeNode{

        friend class TimerTree;

    protected:
        uint64_t Timestamp = 0;
        PromiseCallback *Callback;
        void *Argument;
        TimerTreeNode() = default;
        TimerTreeNode(uint64_t Timestamp, PromiseCallback *Callback, void *Argument) {
            this->Timestamp = Timestamp;
            this->Callback = Callback;
            this->Argument = Argument;
        };
        ~TimerTreeNode() = default;

        RBTreeNode *GetLeft() { return this->Left; }
        RBTreeNode *GetRight() { return this->Right; }

    public:
        virtual int Compare(TimerTreeNode *Node);
        static RBTreeNode *CreateFromAllocator(MemAllocator *Allocator, uint64_t MillSecond, PromiseCallback *Callback, void *Argument);
        static void FreeFromAllocator(MemAllocator *Allocator,RBTreeNode **Node);
    };

    class TimerTree: public RBTree {

    protected:
        TimerTree(MemAllocator *Allocator);
        ~TimerTree() { RBTree::~RBTree(); };
    public:
        static TimerTree* CreateFromAllocator(MemAllocator *ParentAllocator, MemAllocator *Allocator);
        static void FreeFromAllocator(MemAllocator *ParentAllocator, TimerTree **TheRBTree);

        int PostTimerPromise(uint64_t MillSecond, PromiseCallback function, void *args);
        int QueueExpiredTimer(ThreadPool *TPool);
    };
}