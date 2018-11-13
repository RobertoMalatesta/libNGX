#include "Core/Core.h"

using namespace ngx::Core::BasicComponent;

int TimerTreeNode::Compare(TimerTreeNode *Node) {
    if (this->Timestamp == Node->Timestamp) {
        return 0;
    } else if (this->Timestamp > Node->Timestamp) {
        return 1;
    } else {
        return -1;
    }
}

RBTreeNode *TimerTreeNode::CreateFromAllocator(MemAllocator *Allocator, uint64_t Seconds, PromiseCallback *Callback,
                                               void *Argument) {

    RBTreeNode *Ret = nullptr;
    size_t AllocateSize = 0 + sizeof(TimerTreeNode);

    void *PointerToMemory = ((nullptr != Allocator) ? Allocator->Allocate(AllocateSize) : malloc(AllocateSize));

    if (nullptr == PointerToMemory) {
        return nullptr;
    }

    Ret = new(PointerToMemory) TimerTreeNode(Seconds, Callback, Argument);
    return Ret;
}

void TimerTreeNode::FreeFromAllocator(MemAllocator *Allocator, RBTreeNode **Node) {
    if (nullptr != Node && nullptr != *Node) {
        Allocator->Free((void *&) *Node);
    }
}

TimerTree::TimerTree(MemAllocator *Allocator) : RBTree() {

    void *PointerToSentinel = Allocator->Allocate(sizeof(TimerTreeNode));

    if (nullptr == PointerToSentinel) {
        return;
    }

    Root = Sentinel = new(PointerToSentinel) TimerTreeNode();
}

TimerTree::~TimerTree() {

    RBTreeNode *Temp;
    TimerTreeNode *TempTimer;

    for (RBTreeNode *It = Minimum(); It != nullptr && It != Sentinel;) {
        Temp = It, TempTimer = (TimerTreeNode *)It;
        It = Next(It);
        Delete(Temp), Destroy(TempTimer);
    }

    TempTimer = (TimerTreeNode *)(Sentinel);
    Destroy(TempTimer);

    RBTree::~RBTree();
};

TimerTree *TimerTree::CreateFromAllocator(MemAllocator *ParentAllocator, MemAllocator *Allocator) {
    void *PointerToRBTree = ParentAllocator->Allocate(sizeof(TimerTree));

    if (nullptr == PointerToRBTree) {
        return nullptr;
    }

    return new(PointerToRBTree) TimerTree(Allocator);
}

void TimerTree::FreeFromAllocator(MemAllocator *ParentAllocator, TimerTree **TheRBTree) {
    (*TheRBTree)->~TimerTree();
    ParentAllocator->Free((void *&) *TheRBTree);
}

int TimerTree::PostTimerPromise(uint64_t Seconds, PromiseCallback *Callback, void *Argument) {

    uint64_t TimeStamp = GetTimeStamp() + Seconds;

    TimerTreeNode *Timer;

    if (!Build(Timer)) {
        return -1;
    }

    Timer->Timestamp = TimeStamp;
    Timer->Callback = Callback;
    Timer->Argument = Argument;
    Insert(Timer);

    return 0;
}

int TimerTree::QueueExpiredTimer(ThreadPool *TPool) {

    RBTreeNode *It;
    TimerTreeNode *Temp;

    uint64_t Timestamp = GetTimeStamp();

    for (It = Minimum(); It != Sentinel && It != nullptr;) {

        Temp = (TimerTreeNode *) It;
        It = Next(It);

        if (Temp->Timestamp > Timestamp) {
            break;
        }

        TPool->PostPromise(Temp->Callback, Temp->Argument);
        Delete(Temp);
        Destroy(Temp);
    }
    return 0;
}