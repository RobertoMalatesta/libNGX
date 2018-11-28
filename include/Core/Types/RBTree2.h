// https://github.com/forhappy/rbtree/blob/master/rbtree.h

const unsigned RB_RED = 1;
const unsigned RB_BLACK = 0;

class RBNode {
protected:
    unsigned Color:1;
    RBNode *Left, *Right, *Parent;
    inline unsigned GetColor() { return Color; }
    inline unsigned IsRed() { return Color; }
    inline unsigned IsBlack() { return !Color; }
    inline void SetRed() { Color = RB_RED; }
    inline void SetBlack() { Color = RB_BLACK; }
    friend class RBT;
public:
    RBNode(): Color(RB_RED),Left(nullptr), Right(nullptr), Parent(this){};

    inline RBNode *GetLeft(){ return Left; }
    inline RBNode *GetRight() { return Right; }

    virtual int operator > (RBNode &R) = 0;
    virtual int operator < (RBNode &R) = 0;
    virtual int operator == (RBNode &R) = 0;
};

class RBT {
private:
    void RotateLeft(RBNode *Node);
    void RotateRight(RBNode *Node);

    void InsertColor(RBNode *Node);

    void EraseColor(RBNode *Node, RBNode *Parent);

protected:
    RBNode *Root = nullptr;
public:
    RBT() = default;

    int Insert(RBNode *Node);
    void Erase(RBNode *Node);
    virtual RBNode *Find() = 0;

    RBNode *Begin();
    RBNode *End();
    RBNode *Next(RBNode *Node);
};