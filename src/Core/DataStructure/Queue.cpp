#include "Core/Core.h"

using namespace ngx::Core::DataStructure;

Queue::Queue() {
    this->Prev = this -> Next = this;
}

bool Queue::IsEmpty() {
    return this == this->Prev;
}

Queue *Queue::GetNext() {
    return Next;
}

Queue *Queue::GetPrev() {
    return Prev;
}

void Queue::Attach(Queue *Q) {
    Q -> Prev = Prev;
    Q -> Prev -> Next = Q;
    Q -> Next = this;
    Prev = Q;
}

void Queue::Detach() {
    Next->Prev = Prev;
    Prev->Next = Next;
    Prev = Next = this;
}

void Queue::Append(Queue *Node) {
    Node -> Prev = Prev;
    Node -> Prev -> Next = Node;
    Node -> Next = this;
    Prev = Node;
}

void Queue::QueueSplit(Queue *Q, Queue *Q1, Queue *Q2) {
    Q2->Prev = Q->Prev;
    Q2->Prev->Next = Q2;
    Q2->Next = Q1;
    Q->Prev = Q1->Prev;
    Q->Prev->Next = Q;
    Q1->Prev = Q2;
}

void Queue::Sort(Queue *S, int (*cmp)(Queue *, Queue *)) {

    Queue *TQ = S, *TPrev = nullptr, *TNext = nullptr;

    if (S->IsEmpty()) {
        return;
    }

    for (TQ = TQ->GetNext(); TQ != S; TQ = TQ->GetNext()) {

        TPrev = TQ->GetPrev();
        TQ->Detach();

        do {
            if (cmp(TPrev, TQ) <= 0) {
                break;
            }

            TPrev = TPrev->GetPrev();

        } while (TPrev != S);
        TPrev->Append(TQ);
    }
}

Queue *Queue::GetMiddle(Queue *S) {
    Queue *Middle = S, *Next = S;

    if (Middle == S->GetPrev()) {
        return Middle;
    }

    for (;;) {

        Middle = Middle->GetNext();
        Next = Next->GetNext();

        if (Next == S->GetPrev() ||
            (Next = Next->GetNext()) == S->GetPrev()) {
            return Middle;
        }
    }
}
