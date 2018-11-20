// https://github.com/forhappy/rbtree/blob/master/rbtree.h

const bool RB_RED = true;
const bool RB_BLACK = false;

class RBNode {
protected:
    bool Color;
    RBNode *Left, *Right, *Parent;
    inline bool GetColor() { return Color; }
    inline bool IsRed() { return Color; }
    inline bool IsBlack() { return !Color; }
    inline void SetRed() { Color = RB_RED; }
    inline void SetBlack() { Color = RB_BLACK; }
    friend class RBT;
public:
    RBNode(): Color(RB_RED),Left(nullptr), Right(nullptr), Parent(this){};

    inline RBNode *GetLeft(){ return Left; }
    inline RBNode *GetRight() { return Right; }
};

class RBT {
private:
    void inline RotateLeft(RBNode &Node);
    void inline RotateRight(RBNode &Node);
    void InsertColor(RBNode *Node);
protected:
    RBNode Root;
public:
    RBT():Root() {
        Root.Parent = nullptr;
    }

};