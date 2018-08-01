#include "Core/Core.h"

using namespace ngx::Core;

UInt32RBTreeNode::UInt32RBTreeNode() {

}

RBTreeNode *UInt32RBTreeNode::CreateFromAllocator(MemAllocator *Allocator, size_t DateSize) {

    RBTreeNode *Ret = nullptr;
    size_t AllocateSize = DateSize + sizeof(RBTreeNode);

    void *PointerToMemory =  ((nullptr != Allocator)?Allocator->Allocate(AllocateSize):malloc(AllocateSize));

    if (nullptr == PointerToMemory) {
        return nullptr;
    }

    Ret = new (PointerToMemory) UInt32RBTreeNode();
    return Ret;
}

void UInt32RBTreeNode::FreeFromAllocator(MemAllocator *Allocator, RBTreeNode **Node) {

    if (nullptr != Node) {
        Allocator ->Free((void **)Node);
    }
}


int UInt32RBTreeNode::Compare(RBTreeNode *Node) {

    UInt32RBTreeNode *RightOperator = (UInt32RBTreeNode *)Node;
    return Key - RightOperator->Key;
}

RBTree::RBTree(MemAllocator *Allocator) {

    this -> Allocator = Allocator;

    if (nullptr == Allocator) {
        return;
    }
}

RBTree::~RBTree() {

//    if (Sentinel != nullptr) {
//        UInt32RBTreeNode::FreeFromAllocator(this->Allocator, (RBTreeNode **)&Sentinel);
//    }
}

//
//RBTree *RBTree::CreateFromAllocator(MemAllocator *Allocator, size_t DataSize) {
//
//    void * PointerToMemory = Allocator->Allocate(sizeof(RBTree));
//
//    if (nullptr == PointerToMemory) {
//        return nullptr;
//    }
//
//    return new (PointerToMemory) RBTree(Allocator, DataSize);
//}
//
//void RBTree::FreeFromAllocator(MemAllocator *Allocator, RBTree **Tree) {
//
//    if (nullptr != Tree && nullptr != *Tree) {
//        (*Tree)->~RBTree();
//        Allocator -> Free((void **)Tree);
//    }
//}

void RBTree::RotateLeft(ngx::Core::RBTreeNode *Node) {

    RBTreeNode *Temp;

    Temp = Node->Right;
    Node->Right = Temp->Left;

    if (Temp->Left != Sentinel) {
        Temp->Left->Parent = Node;
    }

    Temp->Parent = Node->Parent;

    if (Node == Root) {
        Root = Temp;
    }
    else if (Node == Node->Parent->Left) {
        Node->Parent->Left=Temp;
    }
    else {
        Node->Parent->Right=Temp;
    }

    Temp->Left=Node;
    Node->Parent=Temp;

}

void RBTree::RotateRight(ngx::Core::RBTreeNode *Node) {

    RBTreeNode *Temp;

    Temp = Node->Left;
    Node->Left = Temp->Right;

    if (Temp->Right !=Sentinel) {
        Temp->Right->Parent =Node;
    }

    Temp->Parent = Node->Parent;

    if (Node == Root) {
        Root = Temp;
    }
    else if (Node == Node->Parent->Right) {
        Node->Parent->Right = Temp;
    }
    else {
        Node->Parent->Left = Temp;
    }

    Temp->Right = Node;
    Node->Parent = Temp;
}

void RBTree::Insert(ngx::Core::RBTreeNode *Node) {

    if (Root == Sentinel) {
        Node->Parent = nullptr;
        Node->Left=Sentinel;
        Node->Right=Sentinel;
        Node->SetBlack();
        Root=Node;
        return;
    }

    RBTreeNode *Temp = Root, **P = nullptr;

    for ( ;; ) {

        P = (Node->Compare(Temp)) ? &Temp->Left : & Temp->Right;

        if (*P == Sentinel) {
            break;
        }
    }

    *P = Node;
    Node->Parent = Temp;
    Node->Left=Sentinel;
    Node->Right=Sentinel;
    Node->SetRed();

    while (Node != Root && Node->Parent->IsRed()) {

        if (Node ->Parent == Node->Parent->Parent->Left) {
            Temp = Node->Parent->Parent->Right;

            if (Temp->IsRed()) {
                Node->Parent->SetBlack();
                Temp->SetBlack();
                Node->Parent->Parent->SetRed();
                Node = Node->Parent->Parent;
            }
            else {
                if (Node == Node->Parent->Right) {
                    Node = Node->Parent;
                    RotateLeft(Node);
                }

                Node->Parent->SetBlack();
                Node->Parent->Parent->SetRed();
                RotateRight(Node);
            }
        }
        else {
            Temp = Node->Parent->Parent->Left;

            if (Temp->IsRed()) {
                Node->Parent->SetBlack();
                Temp->SetBlack();
                Node->Parent->Parent->SetRed();
                Node = Node->Parent->Parent;
            }
            else {
                if (Node == Node->Parent->Left) {
                    Node=Node->Parent;
                    RotateRight(Node);
                }
                Node->Parent->SetBlack();
                Node->Parent->Parent->SetRed();
                RotateLeft(Node);
            }
        }

    }
    Root ->SetBlack();
}

RBTreeNode *RBTree::Next(RBTreeNode *Node) {

    RBTreeNode *Ret, *Parent;

    if (Node->Right != Sentinel) {

        Ret = Node->Right;

        while(Ret->Left != Sentinel) {
            Ret = Ret->Left;
        }
        return Ret;
    }

    for ( ;; ) {
        Parent = Node->Parent;

        if (Root == Node) {
            return nullptr;
        }

        if (Node == Parent->Left) {
            return Parent;
        }
        Node = Node -> Parent;
    }
}

void RBTree::Delete(RBTreeNode *Node) {

    RBTreeNode *Subst, *Temp, *W;

    if ( Node->Left == Sentinel) {
        Temp = Node->Right;
        Subst = Node;
    }
    else if (Node->Right == Sentinel) {
        Temp = Node->Left;
        Subst = Node;
    }
    else {
        Subst = Node->Right;

        while (Subst->Left != Sentinel) {
            Subst = Subst->Left;
        }

        if (Subst->Left !=Sentinel) {
            Temp = Subst->Left;
        }
        else {
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
    }
    else {
        Subst->Parent->Right = Temp;
    }

    if (Subst == Node) {
        Temp->Parent = Subst;
    }
    else {

        if (Subst->Parent == Node) {
            Temp->Parent = Subst;
        }
        else {
            Temp->Parent = Subst->Parent;
        }
        Subst->Left = Node->Left;
        Subst->Right = Node->Right;
        Subst->Parent = Node->Parent;
        Subst->CopyColor(Node);

        if (Node == Root) {
            Root = Subst;
        }
        else {
            if (Node == Node->Parent->Left) {
                Node->Parent->Left = Subst;
            }
            else {
                Node->Parent->Right =Subst;
            }
        }

        if (Subst->Left != Sentinel) {
            Subst->Left->Parent =Subst;
        }
        if (Subst->Right != Sentinel) {
            Subst->Right->Parent = Subst;
        }
    }

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
            }

            if (W->Left->IsBlack() && W->Right->IsBlack()) {
                W->SetRed();
                Temp = Temp->Parent;
            }
            else {

                if (W->Right->IsBlack()) {
                    W->Left->IsBlack();
                    W->SetRed();
                    RotateRight(W);
                    W = Temp->Parent->Right;
                }

                W->CopyColor(Temp->Parent);
                Temp->Parent->SetBlack();
                W->Right->SetBlack();
                RotateLeft(Temp->Parent);
                Temp=Root;
            }
        }
        else {

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
            }
            else {

                if (W->Left -> IsBlack()) {
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
