#include "Core/Core.h"

using namespace ngx::Core;

FSEntity::FSEntity(MemAllocator *Allocator, bool Directory): RBTreeNode(), FilterSentinel(nullptr), RegExpSentinel() {
    this->Allocator = Allocator;
    this->Children = nullptr;
    this->Directory = Directory;
}

RBTreeNode* FSEntity::CreateFromAllocator(MemAllocator *Allocator, size_t DateSize) {

    void *PointerToMemory = Allocator->Allocate(DateSize);
    FSEntity *Entity;

    if (nullptr == PointerToMemory) {
        return nullptr;
    }

    Entity = new(PointerToMemory) FSEntity(Allocator);
    return (RBTreeNode *)Entity;
}

void FSEntity::FreeFromAllocator(MemAllocator *Allocator, RBTreeNode **Node) {
    if (nullptr != Node && nullptr != *Node) {
        Allocator ->Free((void **)Node);
    }
}

FSEntity* FSEntity::CreateChild(u_char *Key, size_t Length, size_t DataSize, bool Directory) {

    FSEntity *Node = nullptr;

    if ( !IsDirectory()) {
        return nullptr;
    }

    if (Children == nullptr) {
        Children = FSTree::CreateFromAllocator(Allocator, Allocator);
    }

    return Children->CreateChild(Key, Length, DataSize, Directory);
}

void FSEntity::DeleteChild(u_char *Key, size_t Length, bool Directory) {

    if (Children == nullptr) {
        return;
    }
    return Children->DeleteChild(Key, Length, Directory);
}

int FSEntity::Compare(FSEntity *Node) {

    if (Directory && !Node->Directory) {
        return 1;
    }
    else if (!Directory && Node->Directory) {
        return -1;
    }

    if (KeyLength > Node->KeyLength) {
        return  1;
    }
    else if (KeyLength < Node->KeyLength) {
        return -1;
    }

    if (Hash > Node->Hash) {
        return 1;
    }
    else if (Hash < Node->Hash) {
        return -1;
    }

    return  strncmp((const char *)Key, (const char *)Node->Key, KeyLength);
}

FSTree::FSTree(MemAllocator *Allocator): RBTree(Allocator) {

    void *PointerToSentinel = Allocator->Allocate(sizeof(FSEntity));

    if (nullptr == PointerToSentinel) {
        return;
    }

    Root = Sentinel = new(PointerToSentinel) FSEntity(Allocator);
}

FSTree::~FSTree() {

    RBTreeNode *Temp;

    for (RBTreeNode *It = Minimum(); It != nullptr && It != Sentinel; ) {
        Temp = It;
        It = Next(It);
        Delete(Temp);
        Allocator->Free((void **)&Temp);
    }

    Temp = Sentinel;

    if (Temp != nullptr) {
        Allocator->Free((void **)&Temp);
    }

    RBTree::~RBTree();
}

FSEntity *FSTree::CreateChild(u_char *Key, size_t Length, size_t DataSize, bool Directory = true) {

    FSEntity *Node, *Temp1, *Temp2;

    Temp1 = QueryChild(Key, Length, true);
    Temp2 = QueryChild(Key, Length, false);

    if (Temp1 != nullptr || Temp2 != nullptr) {
        return nullptr;
    }

    Node = (FSEntity *)FSEntity::CreateFromAllocator(Allocator, sizeof(FSEntity) + sizeof(u_char) * (Length + 1) + DataSize);

    if (nullptr == Node) {
        return nullptr;
    }

    Node->Key = (u_char *)Node + sizeof(FSEntity);
    strncpy((char *)Node->Key, (char *)Key, Length);
    Node->KeyLength = Length;
    Node->Hash = murmur_hash2(Node->Key, Length);
    Node->DataSize = DataSize;
    Node->Data = ((u_char *)Node) + sizeof(FSEntity) + sizeof(u_char) * (Length + 1);
    Node->Directory = Directory;

    Insert((RBTreeNode *)Node);

    return Node;
}

FSEntity* FSTree::QueryChild(u_char *Key, size_t Length, bool Directory) {

    FSEntity *Temp, *Node = (FSEntity *)FSEntity::CreateFromAllocator(Allocator, sizeof(FSEntity) + sizeof(u_char) * (Length + 1));

    if (nullptr == Node) {
        return nullptr;
    }

    Node->Key = (u_char *)Node + sizeof(FSEntity);
    strncpy((char *)Node->Key, (char *)Key, Length);

    Node->KeyLength = Length;
    Node->Hash = murmur_hash2(Node->Key, Length);
    Node->Directory = Directory;

    Temp = (FSEntity *)Root;

    int CompareResult;

    do {

        if (Temp == Sentinel) {
            FSEntity::FreeFromAllocator(Allocator, (RBTreeNode **)&Node);
            return nullptr;
        }

        CompareResult = Temp->Compare(Node);

        if (CompareResult > 0) {
            Temp = (FSEntity *)Temp -> GetLeft();
        }
        else if (CompareResult < 0) {
            Temp = (FSEntity *)Temp -> GetRight();
        }
        else {
            FSEntity::FreeFromAllocator(Allocator, (RBTreeNode **)&Node);
            return Temp;
        }
    } while (true);
}

void FSTree::DeleteChild(u_char *Key, size_t Length, bool Directory) {

    FSEntity *Entity = QueryChild(Key, Length, Directory);

    if (nullptr != Entity) {
        Delete((RBTreeNode *)Entity);
        FSEntity::FreeFromAllocator(Allocator, (RBTreeNode **)&Entity);
    }
    return;
}

FSTree* FSTree::CreateFromAllocator(MemAllocator *ParentAllocator, MemAllocator *Allocator) {

    void *PointerToMemory = ParentAllocator->Allocate(sizeof(FSTree));

    if (nullptr == PointerToMemory) {
        return nullptr;
    }

    return new(PointerToMemory) FSTree(Allocator);
}

void FSTree::FreeFromAllocator(MemAllocator *ParentAllocator, FSTree **TheFSTree) {
    if (nullptr != TheFSTree && nullptr != *TheFSTree) {
        (*TheFSTree) -> ~FSTree();
        ParentAllocator ->Free((void **)TheFSTree);
    }
}