#include "Core/Core.h"

using namespace ngx::Core;

FSEntity::FSEntity(bool Directory) : RBTreeNode(), FilterSentinel(nullptr), RegExpSentinel() {
    this->Directory = Directory;
}

FSEntity *FSEntity::CreateChild(u_char *Key, size_t Length, size_t DataSize, bool Directory) {

    if (!IsDirectory()) {
        return nullptr;
    }

    if (ChildrenTree == nullptr) {
//        ChildrenTree = FSTree::CreateFromAllocator(Allocator, Allocator);
    }

    return ChildrenTree->CreateChild(Key, Length, DataSize, Directory);
}

void FSEntity::DeleteChild(u_char *Key, size_t Length, bool Directory) {

    if (ChildrenTree == nullptr) {
        return;
    }
    return ChildrenTree->DeleteChild(Key, Length, Directory);
}

int FSEntity::RawCompare(u_char *Key, size_t Length, bool Directory) {

    if (this->Directory && !Directory) {
        return 1;
    } else if (!this->Directory && Directory) {
        return -1;
    }

    if (KeyLength > Length) {
        return -1;
    } else if (KeyLength < Length) {
        return 1;
    }

    return -strncmp((char *) this->Key, (char *) Key, KeyLength);
}

int FSEntity::Compare(FSEntity *Node) {
    return RawCompare(Node->Key, Node->KeyLength, Node->Directory);
}

FSTree::FSTree(MemAllocator *Allocator) : RBTree() {

    void *PointerToSentinel = Allocator->Allocate(sizeof(FSEntity));

    if (nullptr == PointerToSentinel) {
        return;
    }

    Root = Sentinel = new(PointerToSentinel) FSEntity(Allocator);
}

FSTree::~FSTree() {

    RBTreeNode *Temp;
    FSEntity *TempEntity;

    for (RBTreeNode *It = Minimum(); It != nullptr && It != Sentinel;) {
        Temp = It;
        It = Next(It);
        Delete(Temp);
        Destroy(TempEntity);
    }

    TempEntity = (FSEntity *)Sentinel;
    Destroy(TempEntity);

    RBTree::~RBTree();
}

FSEntity *FSTree::CreateChild(u_char *Key, size_t Length, size_t DataSize, bool Directory = true) {

    FSEntity *Node, *Temp1, *Temp2;

    Temp1 = QueryChild(Key, Length, true);
    Temp2 = QueryChild(Key, Length, false);

    if (Temp1 != nullptr || Temp2 != nullptr) {
        return nullptr;
    }

    Build(Node);


    if (nullptr == Node) {
        return nullptr;
    }

    Node->Key = (u_char *) Node + sizeof(FSEntity) + 4;
    strncpy((char *) Node->Key, (char *) Key, Length);
    Node->KeyLength = Length;
    Node->Hash = murmur_hash2(Node->Key, Length);
    Node->DataSize = DataSize;
    Node->Data = ((u_char *) Node) + sizeof(FSEntity) + 4 + sizeof(u_char) * (Length) + 4;
    Node->Directory = Directory;
    Insert((RBTreeNode *) Node);

    return Node;
}

FSEntity *FSTree::QueryChild(u_char *Key, size_t Length, bool Directory) {

    auto *Temp = (FSEntity *) Root;

    int CompareResult;

    do {

        if (Temp == Sentinel) {
            return nullptr;
        }

        CompareResult = Temp->RawCompare(Key, Length, Directory);

        if (CompareResult < 0) {
            Temp = (FSEntity *) Temp->GetLeft();
        } else if (CompareResult > 0) {
            Temp = (FSEntity *) Temp->GetRight();
        } else {
            return Temp;
        }
    } while (true);
}

void FSTree::DeleteChild(u_char *Key, size_t Length, bool Directory) {

    FSEntity *Entity = QueryChild(Key, Length, Directory);

    if (nullptr != Entity) {
        Delete((RBTreeNode *) Entity);
        Destroy(Entity);
    }
}

FSTree *FSTree::CreateFromAllocator(MemAllocator *ParentAllocator, MemAllocator *Allocator) {

    void *PointerToMemory = ParentAllocator->Allocate(sizeof(FSTree));

    if (nullptr == PointerToMemory) {
        return nullptr;
    }

    return new(PointerToMemory) FSTree(Allocator);
}

void FSTree::FreeFromAllocator(MemAllocator *ParentAllocator, FSTree **TheFSTree) {
    if (nullptr != TheFSTree && nullptr != *TheFSTree) {
        (*TheFSTree)->~FSTree();
        ParentAllocator->Free((void *&) *TheFSTree);
    }
}