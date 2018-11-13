#include "Core/Core.h"

using namespace ngx::Core::DataStructure;

RBTreeNode *RBTree::Minimum() {

    RBTreeNode *Temp = Root;

    if (Temp == Sentinel) {
        return Temp;
    }

    while (Temp->Left != Sentinel) {
        Temp = Temp->Left;
    }

    return Temp;
}

void RBTree::RotateLeft(RBTreeNode *Node) {

    RBTreeNode *Temp;

    Temp = Node->Right;
    Node->Right = Temp->Left;

    if (Temp->Left != Sentinel) {
        Temp->Left->Parent = Node;
    }

    Temp->Parent = Node->Parent;

    if (Node == Root) {
        Root = Temp;
    } else if (Node == Node->Parent->Left) {
        Node->Parent->Left = Temp;
    } else {
        Node->Parent->Right = Temp;
    }

    Temp->Left = Node;
    Node->Parent = Temp;

}

void RBTree::RotateRight(RBTreeNode *Node) {

    RBTreeNode *Temp;

    Temp = Node->Left;
    Node->Left = Temp->Right;

    if (Temp->Right != Sentinel) {
        Temp->Right->Parent = Node;
    }

    Temp->Parent = Node->Parent;

    if (Node == Root) {
        Root = Temp;
    } else if (Node == Node->Parent->Right) {
        Node->Parent->Right = Temp;
    } else {
        Node->Parent->Left = Temp;
    }

    Temp->Right = Node;
    Node->Parent = Temp;
}

void RBTree::Insert(RBTreeNode *Node) {

    if (Node->Left != nullptr || Node->Right != nullptr) {
        return;
    }

    if (Root == Sentinel) {
        Node->Parent = nullptr;
        Node->Left = Sentinel;
        Node->Right = Sentinel;
        Node->SetBlack();
        Root = Node;
        return;
    }

    RBTreeNode *Temp = Root, **P = nullptr;

    for (;;) {

        P = (Node->Compare(Temp) > 0) ? &Temp->Left : &Temp->Right;

        if (*P == Sentinel) {
            break;
        }
        Temp = *P;
    }

    *P = Node;
    Node->Parent = Temp;
    Node->Left = Sentinel;
    Node->Right = Sentinel;
    Node->SetRed();

    while (Node != Root && Node->Parent->IsRed()) {

        if (Node->Parent == Node->Parent->Parent->Left) {

            Temp = Node->Parent->Parent->Right;

            if (Temp->IsRed()) {
                Node->Parent->SetBlack();
                Temp->SetBlack();
                Node->Parent->Parent->SetRed();
                Node = Node->Parent->Parent;
            } else {

                if (Node == Node->Parent->Right) {
                    Node = Node->Parent;
                    RotateLeft(Node);
                }

                Node->Parent->SetBlack();
                Node->Parent->Parent->SetRed();
                RotateRight(Node->Parent->Parent);
            }
        } else {
            Temp = Node->Parent->Parent->Left;

            if (Temp->IsRed()) {
                Node->Parent->SetBlack();
                Temp->SetBlack();
                Node->Parent->Parent->SetRed();
                Node = Node->Parent->Parent;
            } else {

                if (Node == Node->Parent->Left) {
                    Node = Node->Parent;
                    RotateRight(Node);
                }
                Node->Parent->SetBlack();
                Node->Parent->Parent->SetRed();
                RotateLeft(Node->Parent->Parent);
            }
        }

    }
    Root->SetBlack();
}

RBTreeNode *RBTree::Next(RBTreeNode *Node) {

    RBTreeNode *Ret, *Parent;

    if (Node->Right != Sentinel) {

        Ret = Node->Right;

        while (Ret->Left != Sentinel) {
            Ret = Ret->Left;
        }
        return Ret;
    }

    for (;;) {
        Parent = Node->Parent;

        if (Root == Node) {
            return nullptr;
        }

        if (Node == Parent->Left) {
            return Parent;
        }
        Node = Node->Parent;
    }
}

void RBTree::Delete(RBTreeNode *Node) {

    RBTreeNode *Subst, *Temp, *W;

    if (Node->Left == Sentinel) {
        Temp = Node->Right;
        Subst = Node;
    } else if (Node->Right == Sentinel) {
        Temp = Node->Left;
        Subst = Node;
    } else {
        Subst = Node->Right;

        while (Subst->Left != Sentinel) {
            Subst = Subst->Left;
        }

        if (Subst->Left != Sentinel) {
            Temp = Subst->Left;
        } else {
            Temp = Subst->Right;
        }
    }

    if (Subst == Root) {
        Root = Temp;
        Temp->SetBlack();
        Node->Left = nullptr;
        Node->Right = nullptr;
        Node->Parent = nullptr;
        return;
    }

    if (Subst == Subst->Parent->Left) {
        Subst->Parent->Left = Temp;
    } else {
        Subst->Parent->Right = Temp;
    }

    if (Subst == Node) {
        Temp->Parent = Subst->Parent;
    } else {

        if (Subst->Parent == Node) {
            Temp->Parent = Subst;
        } else {
            Temp->Parent = Subst->Parent;
        }
        Subst->Left = Node->Left;
        Subst->Right = Node->Right;
        Subst->Parent = Node->Parent;
        Subst->CopyColor(Node);

        if (Node == Root) {
            Root = Subst;
        } else {
            if (Node == Node->Parent->Left) {
                Node->Parent->Left = Subst;
            } else {
                Node->Parent->Right = Subst;
            }
        }

        if (Subst->Left != Sentinel) {
            Subst->Left->Parent = Subst;
        }
        if (Subst->Right != Sentinel) {
            Subst->Right->Parent = Subst;
        }
    }

    Node->Left = nullptr;
    Node->Right = nullptr;
    Node->Parent = nullptr;

    if (Subst->IsRed()) {
        return;
    }

    while (Temp != Root && Temp->IsBlack()) {

        if (Temp == Temp->Parent->Left) {
            W = Temp->Parent->Right;

            if (W->IsRed()) {
                W->SetBlack();
                Temp->Parent->SetRed();
                RotateLeft(Temp->Parent);
                W = Temp->Parent->Right;
            }

            if (W->Left->IsBlack() && W->Right->IsBlack()) {
                W->SetRed();
                Temp = Temp->Parent;
            } else {
                if (W->Right->IsBlack()) {
                    W->Left->SetBlack();
                    W->SetRed();
                    RotateRight(W);
                    W = Temp->Parent->Right;
                }

                W->CopyColor(Temp->Parent);
                Temp->Parent->SetBlack();
                W->Right->SetBlack();
                RotateLeft(Temp->Parent);
                Temp = Root;
            }
        } else {

            W = Temp->Parent->Left;

            if (W->IsRed()) {
                W->SetBlack();
                Temp->Parent->SetRed();
                RotateRight(Temp->Parent);
                W = Temp->Parent->Left;
            }

            if (W->Left->IsBlack() && W->Right->IsBlack()) {
                W->SetRed();
                Temp = Temp->Parent;
            } else {

                if (W->Left->IsBlack()) {
                    W->Right->SetBlack();
                    W->SetRed();
                    RotateLeft(W);
                    W = Temp->Parent->Left;
                }

                W->CopyColor(Temp->Parent);
                Temp->Parent->SetBlack();
                W->Left->SetBlack();
                RotateRight(Temp->Parent);
                Temp = Root;
            }
        }
    }
    Temp->SetBlack();
}

int UInt32RBTreeNode::Compare(UInt32RBTreeNode *Node) {
    if (this->Key == Node->Key) {
        return 0;
    } else if (this->Key > Node->Key) {
        return 1;
    } else {
        return -1;
    }
}

UInt32RBTree::UInt32RBTree(MemAllocator *Allocator) : RBTree(), AllocatorBuild(Allocator) {

    UInt32RBTreeNode *Temp;

    Build(Temp);

    if (Build(Temp) == 0) {
        Root = Sentinel = Temp;
    }
}

UInt32RBTree::~UInt32RBTree() {

    UInt32RBTreeNode *Temp;

    Temp = (UInt32RBTreeNode *) Sentinel;
    Destroy(Temp);
}

UInt32RBTreeNode *UInt32RBTree::Find(uint32_t Key) {

    auto *Temp = (UInt32RBTreeNode *) Root;

    uint32_t TempKey;

    do {

        if (Temp == Sentinel) {
            return nullptr;
        }

        TempKey = Temp->Key;

        if (Key == TempKey) {
            return Temp;
        } else if (Key > TempKey) {
            Temp = (UInt32RBTreeNode *) Temp->GetRight();
        } else {
            Temp = (UInt32RBTreeNode *) Temp->GetLeft();
        }

    } while (true);
}
