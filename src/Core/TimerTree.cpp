#include "Core/Core.h"

using namespace ngx::Core;

int TimerTreeNode::Compare(TimerTreeNode *Node) {
    if ( this->Timestamp == Node->Timestamp) {
        return 0;
    }
    else if (this->Timestamp > Node->Timestamp) {
        return 1;
    }
    else {
        return -1;
    }
}

RBTreeNode *TimerTreeNode::CreateFromAllocator(MemAllocator *Allocator, uint64_t MillSecond, PromiseCallback *Callback, void *Argument) {

    RBTreeNode *Ret = nullptr;
    size_t AllocateSize = 0 + sizeof(RBTreeNode);

    void *PointerToMemory =  ((nullptr != Allocator)?Allocator->Allocate(AllocateSize):malloc(AllocateSize));

    if (nullptr == PointerToMemory) {
        return nullptr;
    }

    Ret = new (PointerToMemory) TimerTreeNode(MillSecond, Callback, Argument);
    return Ret;
}

void TimerTreeNode::FreeFromAllocator(MemAllocator *Allocator, RBTreeNode **Node) {
    if (nullptr != Node && nullptr != *Node) {
        Allocator ->Free((void **)Node);
    }
}

TimerTree::TimerTree(MemAllocator *Allocator): RBTree(Allocator) {

    void *PointerToSentinel = Allocator->Allocate(sizeof(TimerTreeNode));

    if (nullptr == PointerToSentinel) {
        return;
    }

    Root = Sentinel = new(PointerToSentinel) TimerTreeNode();
}

TimerTree * TimerTree::CreateFromAllocator(MemAllocator *ParentAllocator, MemAllocator *Allocator) {
    void *PointerToRBTree = ParentAllocator->Allocate(sizeof(TimerTree));

    if (nullptr == PointerToRBTree) {
        return nullptr;
    }

    return new(PointerToRBTree) TimerTree(Allocator);
}

void TimerTree::FreeFromAllocator(MemAllocator *ParentAllocator, TimerTree **TheRBTree) {
    (*TheRBTree) -> ~TimerTree();
    ParentAllocator->Free((void **)TheRBTree);
}

int TimerTree::PostTimerPromise(uint64_t MillSecond, PromiseCallback Callback, void *Argument) {

    uint64_t TimeStamp = GetTimeStamp() + MillSecond;

    RBTreeNode *Node = TimerTreeNode::CreateFromAllocator(Allocator, TimeStamp, Callback, Argument);

    if (nullptr == Node) {
        return -1;
    }

    Insert(Node);

    return 0;
}

int TimerTree::QueueExpiredTimer(ThreadPool *TPool) {

    RBTreeNode *It;
    TimerTreeNode *Temp;

    uint64_t Timestamp = GetTimeStamp();

    for (It = Minimum(); It != Sentinel; It = Next(It)) {

        Temp = (TimerTreeNode *)It;

        if (Temp->Timestamp > Timestamp) {
            break;
        }

        TPool->PostPromise(Temp->Callback, Temp->Argument);
        Delete(Temp);
        TimerTreeNode::FreeFromAllocator(Allocator, (RBTreeNode **)&Temp);
    }
    return 0;
}