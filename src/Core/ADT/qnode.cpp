#include "Core/Core.h"

using namespace ngx::Core::ADT;

qnode::qnode() : Prev(this), Next(this) {};
bool qnode::isEmpty() const { return this == this->Prev; }
qnode *qnode::next() const { return Next; }
qnode *qnode::prev() const { return Prev; }

void qnode::append(qnode *Node) {
    Node->Prev = Prev;
    Node->Prev->Next = Node;
    Node->Next = this;
    Prev = Node;
}

void qnode::detach() {
    Next->Prev = Prev;
    Prev->Next = Next;
    Prev = Next = this;
}

void qnode::queueSplit(qnode *Q, qnode *Q1, qnode *Q2) {
    Q2->Prev = Q->Prev;
    Q2->Prev->Next = Q2;
    Q2->Next = Q1;
    Q->Prev = Q1->Prev;
    Q->Prev->Next = Q;
    Q1->Prev = Q2;
}

void qnode::sort(qnode *S, int (*cmp)(qnode *, qnode *)) {
    qnode *TQ = S, *TPrev = nullptr, *TNext = nullptr;

    if (S->isEmpty()) return;

    for (TQ = TQ->next(); TQ != S; TQ = TQ->next()) {
        TPrev = TQ->prev();
        TQ->detach();
        do {
            if (cmp(TPrev, TQ) <= 0) break;
            TPrev = TPrev->prev();
        } while (TPrev != S);
        TPrev->append(TQ);
    }
}

qnode *qnode::getMiddle(qnode *S) {
    qnode *Middle = S, *Next = S;

    if (Middle == S->prev()) return Middle;

    for (;;) {
        Middle = Middle->next();
        Next = Next->next();
        if (Next==S->prev() || (Next = Next->next())==S->prev()) {
            return Middle;
        }
    }
}
