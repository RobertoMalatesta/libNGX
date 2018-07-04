namespace ngx::Core {

    class _RBTreeNode_ {

        private:
            _RBTreeNode_ *Left = nullptr, *Right = nullptr, *Parent = nullptr;
            u_char Color=0;
            uint DataSize=0;
            u_char Data[0];

            bool IsBlack() {return !Color;};
            bool IsRed() {return Color;};
            void SetBlack() { Color=0;};
            void SetRed() {Color=1;};
            void CopyColor(_RBTreeNode_ *Node) { if (nullptr != Node) {Color=Node->Color;}};
            virtual int Compare(_RBTreeNode_ *Node) = 0;

            friend class RBTree;
    };


    class RBTreeNode : public _RBTreeNode_{

        private:
            uint Key = 0;

            virtual int Compare(_RBTreeNode_ *Node);
            static RBTreeNode *CreateFromAllocator(MemAllocator *Allocator, size_t DateSize);
            static void FreeFromAllocator(RBTreeNode *Node);
            friend class RBTree;
    };

//    typedef void *(RBTreeInsertFunc) (RBTreeNode *Root, RBTreeNode *Node, RBTreeNode *Sentinel);

    class RBTree {

        private:
            _RBTreeNode_ *Root;
            _RBTreeNode_ *Sentinel;
            MemAllocator *Allocator;

            void RotateLeft(_RBTreeNode_ *Node);
            void RotateRight(_RBTreeNode_ *Node);

        public:
            RBTree(MemAllocator *Allocator, size_t DataSize);
            void Insert(_RBTreeNode_ *Node);
            void Delete(_RBTreeNode_ *Node);
            _RBTreeNode_ *Next(_RBTreeNode_ *Node);

            static RBTree *CreateFromAllocator(MemAllocator *Allocator);
            static void FreeFromAllocator( RBTree * Tree);
    };
}