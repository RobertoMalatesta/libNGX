#include "Core/Core.h"

using namespace ngx::Core::ADT;

void RBTree::RotateLeft(RBNode *Node) {

    RBNode *Right = Node->Right, *Parent = Node->Parent;

    if ((Node->Right = Right->Left) != nullptr) {
        Right->Left->Parent = Node;
    }
    Right->Left = Node;
    Right->Parent = Parent;

    if (Parent != nullptr) {
        if (Node == Parent->Left) {
            Parent->Left = Right;
        } else {
            Parent->Right = Right;
        }
    } else {
        Root = Right;
    }
    Node->Parent = Right;
}


void RBTree::RotateRight(RBNode *Node) {

    RBNode *Left = Node->Left, *Parent = Node->Parent;

    if ((Node->Left = Left->Right) != nullptr) {
        Left->Right->Parent = Node;
    }
    Left->Right = Node;
    Left->Parent = Parent;
    if (Parent) {
        if (Node == Parent->Right) {
            Parent->Right = Left;
        } else {
            Parent->Left = Left;
        }
    } else {
        Root = Left;
    }
    Node->Parent = Left;
}

void RBTree::InsertColor(RBNode *Node) {

    RBNode *Parent, *GParent;

    while ((Parent = Node->Parent) && Parent->isRed()) {
        GParent = Parent->Parent;
        if (Parent == GParent->Left) {
            {
                RBNode *Uncle = GParent->Right;
                if (Uncle && Uncle->isRed()) {
                    Uncle->setBlack();
                    Parent->setBlack();
                    GParent->setRed();
                    Node = GParent;
                    continue;
                }
            }
            if (Parent->Right == Node) {
                RBNode *Temp;
                RotateLeft(Parent);
                Temp = Parent;
                Parent = Node;
                Node = Temp;
            }
            Parent->setBlack();
            GParent->setRed();
            RotateRight(GParent);
        } else {
            {
                RBNode *Uncle = GParent->Left;

                if (Uncle && Uncle->isRed()) {
                    Uncle->setBlack();
                    Parent->setBlack();
                    GParent->setRed();
                    Node = GParent;
                    continue;
                }
            }

            if (Parent->Left == Node) {
                RBNode *Temp;
                RotateRight(Parent);
                Temp = Parent;
                Parent = Node;
                Node = Temp;
            }
            Parent->setBlack();
            GParent->setRed();
            RotateLeft(GParent);
        }
    }

    Root->setBlack();
}

void RBTree::EraseColor(RBNode *Node, RBNode *Parent) {

    RBNode *Other;

    while ((!Node || Node->isBlack()) && Node != Root) {
        if (Parent->Left == Node) {
            Other = Parent->Right;
            if (Other->isRed()) {
                Other->setBlack();
                Parent->setRed();
                RotateLeft(Parent);
                Other = Parent->Right;
            }
            if ((!Other->Left || Other->Left->isBlack()) &&
                (!Other->Right || Other->Right->isBlack())) {
                Other->setRed();
                Node = Parent;
                Parent = Node->Parent;
            } else {
                if (!Other->Right || Other->Right->isBlack()) {
                    Other->Left->setBlack();
                    Other->setRed();
                    RotateRight(Other);
                    Other = Parent->Right;
                }
                Parent->getColor() ? Other->setBlack() : Other->setRed();
                Parent->setBlack();
                Other->Right->setBlack();
                RotateLeft(Parent);
                Node = Root;
                break;
            }
        } else {
            Other = Parent->Left;
            if (Other->isRed()) {
                Other->setBlack();
                Parent->setRed();
                RotateRight(Parent);
                Other = Parent->Left;
            }
            if ((!Other->Left || Other->Left->isBlack()) &&
                (!Other->Right || Other->Right->isBlack())) {
                Other->setRed();
                Node = Parent;
                Parent = Node->Parent;
            } else {
                if (!Other->Left || Other->Left->isBlack()) {
                    Other->Right->setBlack();
                    Other->setRed();
                    RotateLeft(Other);
                    Other = Parent->Left;
                }
                Parent->getColor() ? Other->setBlack() : Other->setRed();
                Parent->setBlack();
                Other->Left->setBlack();
                RotateRight(Parent);
                Node = Root;
                break;
            }
        }
    }

    if (Node) {
        Node->setBlack();
    }
}

int RBTree::insert(RBNode &Node) {

    RBNode *&Place = Root, *Parent = nullptr;

    while (Place) {

        Parent = Place;

        int Result = Node - *Place;

        if (Result < 0) {
            Place = Place->Left;
        } else if (Result > 0) {
            Place = Place->Right;
        } else {
            Place = Place->Left;
        }
    }

    Node.Parent = Parent;
    Node.Left = Node.Right = nullptr;
    Place = &Node;
    InsertColor(&Node);

    return 1;
}

void RBTree::remove(RBNode *Node) {

    bool Color;
    RBNode *Child, *Parent;

    if (!Node->Left) {
        Child = Node->Right;
    } else if (!Node->Right) {
        Child = Node->Left;
    } else {
        RBNode *Old = Node, *Left;

        Node = Node->Right;

        while ((Left = Node->Left) != nullptr) {
            Node = Left;
        }

        if (Old->Parent) {
            if (Old->Parent->Left == Old) {
                Old->Parent->Left = Node;
            } else {
                Old->Parent->Right = Node;
            }
        } else {
            Root = Node;
        }

        Color = Node->getColor();
        Child = Node->getRight();
        Parent = Node->getParent();

        if (Parent == Old) {
            Parent = Node;
        } else {
            if (Child) {
                Child->Parent = Parent;
            }
            Parent->Left = Child;
            Node->Right = Old->Right;
            Old->Right->Parent = Node;
        }

        Node->Color = Old->getColor();
        Node->Parent = Old->getParent();
        Node->Left = Old->getLeft();
        Old->Left->Parent = Node;

        goto color;
    }

    Color = Node->getColor();
    Parent = Node->getParent();

    if (Child) {
        Child->Parent = Parent;
    }

    if (Parent) {
        if (Parent->Left == Node) {
            Parent->Left = Child;
        } else {
            Parent->Right = Child;
        }
    } else {
        Root = Child;
    }

    color:

    if (Color && Child != nullptr) {
        EraseColor(Child, Parent);
    }

    Node->Parent = Node;
    Node->Left = Node->Right = nullptr;
}

RBNode *RBTree::begin() const {

    RBNode *N;

    if ((N = Root) == nullptr) return nullptr;

    while (N->Left) {
        N = N->Left;
    }

    return N;
}

RBNode *RBTree::end() const {

    RBNode *N;

    if (!(N = Root)) return nullptr;

    while (N->Right) {
        N = N->Right;
    }

    return N;
}

RBNode *RBTree::next(RBNode *Node) const {

    RBNode *Parent;
    Parent = Node->Parent;

    if (Node == Parent) return nullptr;

    if (Node->Right) {
        Node = Node->Right;
        while (Node->Left) {
            Node = Node->Left;
        }
        return Node;
    }

    while ((Parent = Node->Parent) && Node == Parent->Right) {
        Node = Parent;
    }

    return Parent;
}

RBNode *RBTree::prev(RBNode *Node) const {

    RBNode *Parent;

    if (Node->Parent == Node) return nullptr;

    if (Node->Left) {
        Node = Node->Left;
        while (Node->Right) {
            Node = Node->Right;
        }
        return Node;
    }

    while ((Parent = Node->Parent) && Node == Parent->Left) {
        Node = Parent;
    }

    return Parent;
}
