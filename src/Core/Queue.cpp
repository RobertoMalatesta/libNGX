#include "Core/Core.h"

// Not test yet

namespace ngx::Core {


    Queue::Queue() {
        this -> Sentinel = this;
    }

    Queue::Queue(Queue *Sentinel, bool IsInsertToHead) {
        this -> Sentinel = Sentinel;
        IsInsertToHead? InsertToHead(): InsertToTail();
    }

    bool Queue::IsEmpty() {
        return this == this -> Prev;
    }

    void Queue::InsertToHead() {
        Next = Sentinel -> Next;
        Next -> Prev = Next;
        Prev = Sentinel;
        Sentinel -> Next = this;
    }

    void Queue::InsertToTail() {

        Prev = Sentinel -> Prev;
        Prev->Next = this;
        Next = Sentinel;
        Sentinel -> Prev = this;
    }

    Queue *Queue::GetHead() {
        return Sentinel -> Next;
    }

    Queue *Queue::GetLast(){
        return Sentinel -> Prev;
    }

    Queue *Queue::GetSentinel(){
        return Sentinel;
    }

    Queue *Queue::GetNext(){
        return Next;
    }

    Queue *Queue::GetPrev(){
        return Prev;
    }

    void Queue::Attach() {
        Sentinel -> Prev -> Next = Next;
        Next -> Prev = Sentinel -> Prev;
        Sentinel -> Prev = Prev;
        Sentinel -> Prev ->Next = Sentinel;
    }

    void Queue::Detach() {
        Next -> Prev = Prev;
        Prev -> Next = Next;
        Prev = Next = this;
    }

    void Queue::Append(Queue *Node) {

        if (nullptr == Node) {
            return;
        }

        Node -> Next = Next;
        Node -> Next -> Prev = Node;
        Node -> Prev = this;
        Next = Node;
    }

    void Queue::QueueSplit(Queue *Q1, Queue *Node) {

        Node -> Prev = Sentinel -> Prev;
        Node -> Prev -> Next = Node;
        Node -> Next = Q1;
        Sentinel -> Prev = Q1 -> Prev;
        Sentinel -> Prev -> Next = Sentinel;
        Q1 -> Prev = Node;
    }

    void Queue::Sort(int (*cmp)(Queue *, Queue *) ) {

        Queue *TQueue=Sentinel, *TPrev= nullptr, *TNext = nullptr;

        if (GetLast() == GetHead()) {
            return;
        }

        for (TQueue = TQueue -> GetNext(); TQueue != Sentinel; TQueue = TQueue -> GetNext()) {

            TPrev = TQueue -> GetPrev();
            TQueue->Detach();

            do {
                if (cmp(TPrev, TQueue) <= 0) {
                    break;
                }

                TPrev = TPrev -> GetPrev();

            } while (TPrev != Sentinel);
            TPrev -> Append(TQueue);
        }
    }

    Queue* Queue::GetMiddle() {
        Queue *Middle = Sentinel, *Next = Sentinel;

        if (Middle == GetLast()) {
            return Middle;
        }

        for ( ;; ) {

            Middle = Middle -> GetNext();
            Next = Next -> GetNext();

            if (Next == GetLast() ||
                    (Next = Next -> GetNext()) == GetLast()) {
                return  Middle;
            }
        }
    }
}