namespace ngx::Core {

    class RBTreeNode {

        private:
            uint Key;
            RBTreeNode *Left, *Right, *Parent;
            u_char Color;
            uint DataSize;
            u_char Data[0];

        public:

            RBTreeNode();
            bool IsBlack();
            bool IsRed();
            void SetBlack();
            void SetRed();
            void CopyColor(RBTreeNode *Node);
            int Compare(RBTreeNode *Node);

            static RBTreeNode *CreateFromAllocator(MemAllocator *Allocator, uint DateSize);
            static void FreeFromAllocator(RBTreeNode *Node);

            friend class RBTree;
    };

    typedef void *(RBTreeInsertFunc) (RBTreeNode *Root, RBTreeNode *Node, RBTreeNode *Sentinel);

    class RBTree {

        private:
            RBTreeNode *Root;
            RBTreeNode *Sentinel;
            RBTreeInsertFunc InsertFunc;
            MemAllocator *Allocator;

        public:
            RBTree(MemAllocator *Allocator);
            void Insert(RBTreeNode *Node);
            void Delete(RBTreeNode *Node);
            RBTreeNode *Next(RBTreeNode *Node);

            static RBTree *CreateFromAllocator(MemAllocator *Allocator);
            static void FreeFromAllocator( RBTree * Tree);
    };
}