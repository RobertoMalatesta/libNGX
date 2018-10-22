namespace ngx::Core {

    typedef int (FilterCallback)(FSEntity *Entity, void *Argument);

    class FilterQueue : Queue {

    private:

        friend class FSEntity;

        FilterCallback *Callback = nullptr;

        FilterQueue() {};

        FilterQueue(FilterCallback *Callback) {};

        ~FilterQueue() {};

    public:
        static FilterQueue *CreateFromAllocator(MemAllocator *Allocator);

        static void FreeFromAllocator(MemAllocator *Allocator, FilterQueue **PointerToFilterQueue);

        static int Filter(FilterQueue *Filter, FSEntity *Entity, void *Argument);
    };

    class RegExpQueue : Queue {

    private:

        friend class FSEntity;

        u_char *RegExp = nullptr;
        size_t RegExpLength = 0;
        size_t DataLength = 0;
        u_char Data[0];

        RegExpQueue *Match(u_char *Key) {
            bool Matched;
            if ((RegExp != nullptr) && !(Matched = true)) {
                return this;
            }
            return nullptr;
        }

        RegExpQueue() {};

        RegExpQueue(u_char RegExp, size_t Length, size_t DataSize) {};

        ~RegExpQueue() {};

    public:
        static RegExpQueue *CreateFromAllocator(MemAllocator *Allocator);

        static void FreeFromAllocator(MemAllocator *Allocator, RegExpQueue **PointerToRegExpQueue);

        static int Match(RegExpQueue *RegExpQueue, u_char *Key, size_t Length);
    };

    class FSEntity : RBTreeNode {

    protected:
        friend class FSTree;

        MemAllocator *Allocator;
        bool Directory = false;
        FSTree *Children;
        FilterQueue FilterSentinel;
        RegExpQueue RegExpSentinel;
        size_t KeyLength = -1;
        size_t DataSize = -1;
        uint32_t Hash = 0L;
        u_char *Key;
        u_char *Data;

        FSEntity(MemAllocator *Allocator, bool Directory = false);

        ~FSEntity();

        int RawCompare(u_char *Key, size_t Length, bool Directory);

        virtual int Compare(FSEntity *Node);

        RBTreeNode *GetLeft() { return this->Left; }

        RBTreeNode *GetRight() { return this->Right; }

        FSEntity *CreateChild(u_char *Key, size_t Length, size_t DataSize, bool Directory);

        void DeleteChild(u_char *Key, size_t Length, bool Directory);

    public:
        bool IsDirectory() { return Directory; };

        u_char *GetDataPointer() { return Data; };

        static RBTreeNode *CreateFromAllocator(MemAllocator *Allocator, size_t DateSize);

        static void FreeFromAllocator(MemAllocator *Allocator, RBTreeNode **Node);
    };

    class FSTree : public RBTree {

    protected:
        FSTree(MemAllocator *Allocator);

        ~FSTree();

    public:
        FSEntity *CreateChild(u_char *Key, size_t Length, size_t DataSize, bool Directory);

        FSEntity *QueryChild(u_char *Key, size_t Length, bool Directory);

        void DeleteChild(u_char *Key, size_t Length, bool Directory);

        // CreatePath
        FSEntity *DeletePath(u_char, size_t Length);

        static FSTree *CreateFromAllocator(MemAllocator *ParentAllocator, MemAllocator *Allocator);

        static void FreeFromAllocator(MemAllocator *ParentAllocator, FSTree **TheRBTree);
    };
}
