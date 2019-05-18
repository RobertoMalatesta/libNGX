//
// https://github.com/forhappy/rbtree/blob/master/rbtree.h

const bool RB_RED = false;
const bool RB_BLACK = true;

class RBNode {
protected:
    bool Color;
    RBNode *Left, *Right, *Parent;

    inline bool getColor() const { return Color; }

    inline bool isRed() const { return !Color; }

    inline bool isBlack() const { return Color; }

    inline void setRed() { Color = RB_RED; }

    inline void setBlack() { Color = RB_BLACK; }

    friend class RBTree;

public:
    RBNode() : Color(RB_RED), Left(nullptr), Right(nullptr), Parent(this) {};

    inline RBNode *getLeft() const { return Left; }

    inline RBNode *getRight() const { return Right; }

    inline RBNode *getParent() const { return Parent; }

    virtual int operator-(RBNode &R) = 0;
};

class RBTree {
private:
    void RotateLeft(RBNode *Node);

    void RotateRight(RBNode *Node);

    void InsertColor(RBNode *Node);

    void EraseColor(RBNode *Node, RBNode *Parent);

protected:
    RBNode *Root = nullptr;
public:
    RBTree() = default;

    int insert(RBNode &Node);

    void remove(RBNode *Node);

    RBNode *begin() const;

    RBNode *end() const;

    RBNode *next(RBNode *Node) const;

    RBNode *prev(RBNode *Node) const;
};
