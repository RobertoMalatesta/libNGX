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
    friend class RBT;
public:
    RBNode(): Color(RB_RED),Left(nullptr), Right(nullptr), Parent(this){};

    inline RBNode *GetLeft(){ return Left; }
    inline RBNode *GetRight() { return Right; }

    virtual int operator - (RBNode &R) = 0;
};

class UInt32RBNode: public RBNode {
protected:
    uint32_t Key;

public:

    UInt32RBNode(uint32_t Key): RBNode(), Key(Key) {}

    uint32_t GetKey() { return Key; }

    virtual int operator - (RBNode &R) {
       return Key -  ((UInt32RBNode &)R).Key;
    }

    virtual int operator - (uint32_t R) {
        return this->Key - R;
    }
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

    RBNode *Begin();
    RBNode *End();
    RBNode *Next(RBNode *Node);
};

class UInt32RBT : public RBT {

public:
    virtual UInt32RBNode *Find(uint32_t Key);
};