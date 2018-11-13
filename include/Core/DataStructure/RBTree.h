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

    RBTreeNode *GetLeft() { return this->Left; }

    RBTreeNode *GetRight() { return this->Right; }

public:
    friend class RBTree;
};

class RBTree {

protected:
    RBTreeNode *Root = nullptr;
    RBTreeNode *Sentinel = nullptr;

    RBTree() = default;

    ~RBTree() = default;

    void RotateLeft(RBTreeNode *Node);

    void RotateRight(RBTreeNode *Node);

public:
    void Insert(RBTreeNode *Node);

    void Delete(RBTreeNode *Node);

    RBTreeNode *Next(RBTreeNode *Node);

    RBTreeNode *Minimum();
};

union UInt32TreeValue {
    void *Ptr;
    uint32_t UInt;
};


class UInt32RBTreeNode : public RBTreeNode {

    friend class UInt32RBTree;

protected:
    uint32_t Key = 0;

public:

    UInt32TreeValue Value;

    UInt32RBTreeNode() = default;

    ~UInt32RBTreeNode() = default;

    void SetKey(uint32_t Key) { this->Key = Key; };

    virtual int Compare(UInt32RBTreeNode *Node);
};

class UInt32RBTree : public RBTree, public AllocatorBuild<UInt32RBTreeNode> {

public:
    UInt32RBTree(MemAllocator *Allocator);

    ~UInt32RBTree();

    UInt32RBTreeNode *Find(uint32_t Key);
};

// [TODO]
// UInt32RBTree 哈希或整数检索树
// TimerTree 定时器检索树 定时发出Promise
// FSTree 文件系统树 层级资源索引, 正则支持 要用哈希来优化加速
// CacheTree 数据缓存树,版本控制+过期清除
// PrefixSuffixTree 前缀后缀查询树
