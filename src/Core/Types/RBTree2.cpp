#include "Core/Core.h"

using namespace ngx::Core::Types;

void RBT::RotateLeft(RBNode *Node) {


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


void RBT::RotateRight(RBNode *Node) {

    RBNode *Left = Node->Left, *Parent = Node->Parent;

    if ((Node->Left = Left->Right) != nullptr) {

        Left->Parent = Node;
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

void RBT::InsertColor(RBNode *Node) {

    RBNode *Parent, *GParent;

    while ((Parent = Node->Parent) && Parent->IsRed()) {

        GParent = Parent->Parent;

        if (Parent == GParent->Left) {
            {
                RBNode *Uncle = GParent->Right;

                if (Uncle && Uncle->IsRed()) {

                    Uncle->SetBlack();
                    Parent->SetBlack();
                    GParent->SetRed();
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

            Parent->SetBlack();
            GParent->SetRed();
            RotateRight(GParent);
        } else {
            {
                RBNode *Uncle = GParent->Left;

                if (Uncle && Uncle->IsRed()) {

                    Uncle->SetBlack();
                    Parent->SetBlack();
                    GParent->SetRed();
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

            Parent->SetBlack();
            GParent->SetRed();
            RotateLeft(GParent);
        }
    }

    Root->SetBlack();
}

void RBT::EraseColor(RBNode *Node, RBNode *Parent) {

    RBNode *Other;

    while ((!Node || Node->IsBlack()) && Node != Root) {

        if (Parent->Left == Node) {

            Other = Parent->Right;

            if (Other->IsRed()) {

                Other->SetBlack();
                Parent->SetRed();
                RotateLeft(Parent);
                Other = Parent->Right;
            }

            if ((!Other->Left || Other->Left->IsBlack()) &&
                (!Other->Right || Other->Right->IsBlack())) {

                Other->SetRed();
                Node = Parent;
                Parent = Node->Parent;
            } else {

                if (!Other->Right || Other->Right->IsBlack()) {
                    Other->Left->SetBlack();
                    Other->SetRed();
                    RotateRight(Other);
                    Other = Parent->Right;
                }

                Parent->GetColor() == RB_RED ? Other->SetRed() : Other->SetBlack();

                Parent->SetBlack();
                Other->Right->SetBlack();
                RotateLeft(Parent);
                Node = Root;
                break;
            }
        } else {
            Other = Parent->Left;
            if (Other->IsRed()) {

                Other->SetBlack();
                Parent->SetRed();
                RotateRight(Parent);
                Other = Parent->Left;
            }

            if ((!Other->Left || Other->Left->IsBlack()) &&
                (!Other->Right || Other->Right->IsBlack())) {

                Other->SetRed();
                Node = Parent;
                Parent = Node->Parent;
            } else {

                if (!Other->Left || Other->Left->IsBlack()) {
                    Other->Right->SetBlack();
                    Other->SetBlack();
                    RotateLeft(Other);
                    Other = Parent->Left;
                }

                Parent->GetColor() == RB_RED ? Other->SetRed() : Other->SetBlack();
                Parent->SetBlack();
                Other->Left->SetBlack();
                RotateRight(Parent);
                Node = Root;
                break;
            }

        }
    }

    if (Node) {
        Node->SetBlack();
    }
}

int RBT::Insert(RBNode *Node) {

    RBNode **Place = &Root, *Parent = nullptr;

    while (*Place) {

        Parent = *Place;

        int Result = *Node -**Place;

        if (Result < 0) {
            Place = &(*Place)->Left;
        } else if (Result > 0){
            Place = &(*Place)->Right;
        } else {
            Place = &(*Place)->Left;
        }
    }

    Node->Parent = Parent;
    Node->Left = Node->Right = nullptr;
    *Place = Node;
    InsertColor(Node);

    return 1;
}

void RBT::Erase(RBNode *Node) {

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

        Color = Node->Color;
        Child = Node->Right;
        Parent = Node->Parent;

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

        Node->Color = Old->Color;
        Node->Parent = Old->Parent;
        Node->Left = Old->Left;
        Old->Left->Parent = Node;

        goto color;
    }

    Color = Node->Color;
    Parent = Node->Parent;

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
    if (Color) {
        EraseColor(Child, Parent);
    }
}

RBNode *RBT::Begin() {

    RBNode *N = Root;

    if (N == nullptr) {
        return nullptr;
    }

    while (N->Left) {
        N = N->Left;
    }

    return N;
}

RBNode *RBT::End() {

    RBNode *N = Root;

    if (!N) {
        return nullptr;
    }

    while (N->Right) {
        N = N->Right;
    }

    return N;
}

RBNode* RBT::Next(RBNode *Node) {

    RBNode *Parent;

    Parent = Node->Parent;

    if (Node == Parent) {
        return nullptr;
    }

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

RBNode* RBT::Prev(RBNode *Node) {

    RBNode *Parent;

    if (Node->Parent == Node) {
        return nullptr;
    }

    if (Node -> Left) {
        Node = Node->Left;
        while(Node->Right) {
            Node = Node->Right;
        }
        return Node;
    }

    while((Parent = Node->Parent) && Node == Parent->Left) {
        Node = Parent;
    }

    return Parent;
}

UInt32RBNode* UInt32RBT::Find(uint32_t Key) {

    RBNode *Place = Root;

    while (Place) {

        int Result = ((UInt32RBNode *)Place)->operator-(Key);

        if (Result < 0) {
            Place = Place->GetLeft();
        } else if (Result > 0){
            Place = Place->GetRight();
        } else {
            return (UInt32RBNode *)Place;
        }
    }

    return nullptr;
}