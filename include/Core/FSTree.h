namespace ngx::Core {

    typedef int (FilterCallback)(FSEntity *Entity, void *Argument);

    class FilterQueue: Queue {

        private:

            friend class FSEntity;
            FilterCallback *Callback;

            FilterQueue(FilterCallback *Callback, size_t DataSize);
            ~FilterQueue();

        public:
            static FilterQueue *CreateFromAllocator(MemAllocator *Allocator);
            static void FreeFromAllocator(MemAllocator *Allocator, FilterQueue **PointerToFilterQueue);
            static int Filter(FilterQueue *Filter, FSEntity *Entity, void *Argument);
    };

    class RegExpQueue: Queue {

        private:

            friend class FSEntity;
            u_char Data[0];
            RegExpQueue *Match(u_char *Key) {
                bool Matched;
                if (!(Matched = true)) {
                    return this;
                }
                return nullptr;
            }

            RegExpQueue( u_char RegExp, size_t Length, size_t DataSize);
            ~RegExpQueue();

        public:
            static RegExpQueue *CreateFromAllocator(MemAllocator *Allocator);
            static void FreeFromAllocator(MemAllocator *Allocator, RegExpQueue **PointerToRegExpQueue);
            static int Match(RegExpQueue *RegExpQueue, u_char *Key);

    };

    class FSEntity : RBTreeNode{

        protected:
            friend class FSTree;
            MemAllocator *Allocator;
            FSTree *Children;
            FilterQueue  FilterSentinel;
            RegExpQueue  RegExpSentinel;
            size_t KeyLength = -1;
            size_t DataSize = -1;
            uint32_t Hash = 0LL;
            u_char Key[0];
            u_char *Data;

            FSEntity();
            ~FSEntity();

            virtual int Compare(FSEntity *Node);
            FSEntity *CreateChildDirectory(u_char *Key, size_t Length, size_t DataSize);
            FSEntity *CreateChildNode(u_char *Key, size_t Length, size_t DataSize);
            void * DeleteChildEntity(u_char *Key, size_t Length);

        public:
            bool IsDirectory() {return Children == nullptr;};
            u_char *GetDataPointer();
    };

    class FSTree : RBTree {

        private:
            MemAllocator *Allocator;

        protected:
            FSTree(MemAllocator *Allocator);
            ~FSTree();

        public:
            static FSTree *CreateFSTree(MemAllocator *ParentAllocator, MemAllocator *Allocator);
            static void FreeFSTree(FSTree **PointerToFSTree);

            FSEntity *CreateDirectory(u_char *Path, size_t Length, size_t DataSize);
            FSEntity *CreateNode(u_char *Path, size_t Length, size_t DataSize);

            FSEntity *QueryPath(u_char *Path, size_t Length);
            FSEntity *DeletePath(u_char, size_t Length);
    };
}
