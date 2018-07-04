#include "Core/Core.h"

using namespace ngx::Core;

RBTreeNode *RBTreeNode::CreateFromAllocator(MemAllocator *Allocator, size_t DateSize) {

    RBTreeNode *Ret = nullptr;
    size_t AllocateSize = DateSize + sizeof(RBTreeNode);

    void *PointerToMemory =  ((nullptr != Allocator)?Allocator->Allocate(AllocateSize):malloc(AllocateSize));

    if (nullptr == PointerToMemory) {
        return nullptr;
    }

    Ret = new (PointerToMemory) RBTreeNode();
    return Ret;
}

void RBTreeNode::FreeFromAllocator(RBTreeNode *Node) {
}

int RBTreeNode::Compare(_RBTreeNode_ *Node) {
    return 0;
}

RBTree::RBTree(MemAllocator *Allocator, size_t DataSize) {

    this -> Allocator = Allocator;

    if (nullptr == Allocator) {
        return;
    }

    Sentinel = RBTreeNode::CreateFromAllocator(this->Allocator, DataSize);

    if (nullptr == Sentinel) {
        return;
    }

    Sentinel->SetBlack();
    Root = Sentinel;
}

void RBTree::Insert(ngx::Core::_RBTreeNode_ *Node) {

    if (Root == Sentinel) {
        Node->Parent = nullptr;
        Node->Left=Sentinel;
        Node->Right=Sentinel;
        Node->SetBlack();
        Root=Node;
        return;
    }

    _RBTreeNode_ *Temp = Root, **P = nullptr;

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

void RBTree::RotateLeft(ngx::Core::_RBTreeNode_ *Node) {

    _RBTreeNode_ *Temp;

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

void RBTree::RotateRight(ngx::Core::_RBTreeNode_ *Node) {

    _RBTreeNode_ *Temp;

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

_RBTreeNode_ *RBTree::Next(ngx::Core::_RBTreeNode_ *Node) {

    _RBTreeNode_ *Ret, *Parent;

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