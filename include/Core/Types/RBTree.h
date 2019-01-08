//
// https://github.com/forhappy/rbtree/blob/master/rbtree.h

const bool RB_RED = false;
const bool RB_BLACK = true;

class RBNode {
protected:
    bool Color;
    RBNode *Left, *Right, *Parent;

    inline bool GetColor() { return Color; }

    inline bool IsRed() { return !Color; }

    inline bool IsBlack() { return Color; }

    inline void SetRed() { Color = RB_RED; }

    inline void SetBlack() { Color = RB_BLACK; }

    friend class RBTree;

public:
    RBNode() : Color(RB_RED), Left(nullptr), Right(nullptr), Parent(this) {};

    inline RBNode *GetLeft() { return Left; }

    inline RBNode *GetRight() { return Right; }

    inline RBNode *GetParent() { return Parent; }

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

    int Insert(RBNode *Node);

    void Erase(RBNode *Node);

    RBNode *Begin();

    RBNode *End();

    RBNode *Next(RBNode *Node);

    RBNode *Prev(RBNode *Node);
};
