namespace ngx::Core {

    class RBTreeNode {

        private:
            RBTreeNode *Left = nullptr, *Right = nullptr, *Parent = nullptr;
            u_char Color=0;
            uint DataSize=0;
            u_char Data[0];

            bool IsBlack() {return !Color;};
            bool IsRed() {return Color;};
            void SetBlack() { Color=0;};
            void SetRed() {Color=1;};
            void CopyColor(RBTreeNode *Node) { if (nullptr != Node) {Color=Node->Color;}};

        public:
            virtual int Compare(RBTreeNode *Node) = 0;
            static RBTreeNode *CreateFromAllocator(MemAllocator *Allocator, size_t DateSize) {return nullptr;};
            static void FreeFromAllocator(MemAllocator *Allocator, RBTreeNode **Node) {};

            friend class RBTree;
    };

    class UInt32RBTreeNode : public RBTreeNode{

        private:
            uint32_t Key = 0;
            UInt32RBTreeNode();
            ~UInt32RBTreeNode();

        public:
            virtual int Compare(RBTreeNode *Node);
            static RBTreeNode *CreateFromAllocator(MemAllocator *Allocator, size_t DateSize);
            static void FreeFromAllocator(MemAllocator *Allocator,RBTreeNode **Node);

        friend class RBTree;
    };

//    typedef void *(RBTreeInsertFunc) (RBTreeNode *Root, RBTreeNode *Node, RBTreeNode *Sentinel);

    class RBTree {

        private:
            RBTreeNode *Root;
            RBTreeNode *Sentinel;
            MemAllocator *Allocator;

            void RotateLeft(RBTreeNode *Node);
            void RotateRight(RBTreeNode *Node);
            RBTree(MemAllocator *Allocator);
            ~RBTree();
        public:
            void Insert(RBTreeNode *Node);
            void Delete(RBTreeNode *Node);
            RBTreeNode *Next(RBTreeNode *Node);
    };

    class UInt32RBTree: public RBTree {

    private:

        UInt32RBTree(MemAllocator *Allocator, size_t DataSize);
        ~UInt32RBTree();
    public:
        void Insert(RBTreeNode *Node);
        void Delete(RBTreeNode *Node);
        RBTreeNode *Next(RBTreeNode *Node);

        static RBTree* CreateFromAllocator(MemAllocator *Allocator);
        static void FreeFromAllocator(MemAllocator *Allocator, UInt32RBTree *TheRBTree);

        static RBTreeNode* CreateNodeFromAllocator(MemAllocator *Allocator);
        static void FreeNodeFromAllocator(MemAllocator *Allocator, UInt32RBTreeNode *TheRBTreeNode);
    };
}

// [TODO]
// UInt32RBTree 哈希或整数检索树
// TimerTree 定时器检索树 定时发出Promise
// FSTree 文件系统树 层级资源索引, 正则支持 要用哈希来优化加速
// CacheTree 数据缓存树,版本控制+过期清除
// PrefixSuffixTree 前缀后缀查询树