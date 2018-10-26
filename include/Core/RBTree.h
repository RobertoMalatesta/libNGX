namespace ngx{
    namespace Core {

        class RBTreeNode {

        protected:
            RBTreeNode *Left = nullptr, *Right = nullptr, *Parent = nullptr;
            u_char Color = 0;

            bool IsBlack() { return !Color; };

            bool IsRed() { return Color; };

            void SetBlack() { Color = 0; };

            void SetRed() { Color = 1; };

            void CopyColor(RBTreeNode *Node) { if (nullptr != Node) { Color = Node->Color; }};

            virtual int Compare(RBTreeNode *Node) { return 0; }

        public:
            static RBTreeNode *CreateFromAllocator(MemAllocator *Allocator, size_t DateSize) { return nullptr; };

            static void FreeFromAllocator(MemAllocator *Allocator, RBTreeNode **Node) {};

            friend class RBTree;
        };

        class UInt32RBTreeNode : public RBTreeNode {

            friend class UInt32RBTree;

        protected:
            uint32_t Key = 0;
            size_t DataSize = 0;
            u_char Data[0];

            UInt32RBTreeNode() = default;

            UInt32RBTreeNode(size_t DataSize, uint32_t Key) {
                this->DataSize = DataSize;
                this->Key = Key;
            };

            ~UInt32RBTreeNode() = default;

            RBTreeNode *GetLeft() { return this->Left; }

            RBTreeNode *GetRight() { return this->Right; }

        public:

            void SetKey(uint32_t Key) { this->Key = Key; };

            uint32_t GetKey() { return Key; };

            u_char *GetDataPointer() { return this->Data; };

            virtual int Compare(UInt32RBTreeNode *Node);

            static RBTreeNode *CreateFromAllocator(MemAllocator *Allocator, size_t DateSize);

            static void FreeFromAllocator(MemAllocator *Allocator, RBTreeNode **Node);
        };

        class RBTree {

        protected:
            RBTreeNode *Root;
            RBTreeNode *Sentinel;
            MemAllocator *Allocator;

            RBTree(MemAllocator *Allocator);

            ~RBTree() = default;

            void RotateLeft(RBTreeNode *Node);

            void RotateRight(RBTreeNode *Node);

        public:
            void Insert(RBTreeNode *Node);

            void Delete(RBTreeNode *Node);

            RBTreeNode *Next(RBTreeNode *Node);

            RBTreeNode *Minimum();
        };

        class UInt32RBTree : public RBTree {

        protected:
            UInt32RBTree(MemAllocator *Allocator);

            ~UInt32RBTree();

        public:
            UInt32RBTreeNode *CreateNodeFromAllocator(size_t DataSize, uint32_t Key = 0);

            void FreeNodeFromAllocator(UInt32RBTreeNode **TheRBTreeNode);

            static UInt32RBTree *CreateFromAllocator(MemAllocator *ParentAllocator, MemAllocator *Allocator);

            static void FreeFromAllocator(MemAllocator *ParentAllocator, UInt32RBTree **TheRBTree);

            UInt32RBTreeNode *Find(uint32_t Key);
        };
    }
}

// [TODO]
// UInt32RBTree 哈希或整数检索树
// TimerTree 定时器检索树 定时发出Promise
// FSTree 文件系统树 层级资源索引, 正则支持 要用哈希来优化加速
// CacheTree 数据缓存树,版本控制+过期清除
// PrefixSuffixTree 前缀后缀查询树