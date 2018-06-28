namespace ngx::Core {

    class RBTreeNode {

        private:
            uint Key;
            RBTreeNode *Left, *Right, *Parent;
            u_char Color;
            u_char Data;

        public:

            RBTreeNode();
            bool IsBlack();
            bool IsRed();
            void SetBlack();
            void SetRed();
            void CopyColor(RBTreeNode *Node);

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
            static RBTree *CreateFromAllocator(MemAllocator *Allocator);
            void Insert(RBTreeNode *Node);
            void Delete(RBTreeNode *Node);
            RBTreeNode *Next(RBTreeNode *Node);
    };


}