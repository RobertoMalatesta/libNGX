#ifndef LIBNGX_CORE_ADT_QUEUE
#define LIBNGX_CORE_ADT_QUEUE
namespace ngx {
namespace Core {
namespace ADT {

class qnode {
protected:
    qnode *Prev, *Next;

public:
    qnode() : Prev(this), Next(this) {};
    bool isEmpty() const { return this==Prev; };
    qnode *next() const { return Next; };
    qnode *prev() const { return Prev; };
    void append(qnode *Node) {
        Node->Prev = Prev;
        Node->Prev->Next = Node;
        Node->Next = this;
        Prev = Node;
    };
    void detach() {
        Next->Prev = Prev;
        Prev->Next = Next;
        Prev = Next = this;
    }
    static void queueSplit(qnode *Q, qnode *Q1, qnode *Q2) {
        Q2->Prev = Q->Prev;
        Q2->Prev->Next = Q2;
        Q2->Next = Q1;
        Q->Prev = Q1->Prev;
        Q->Prev->Next = Q;
        Q1->Prev = Q2;
    }
    static void sort(qnode *S, int (*cmp)(qnode *q1, qnode *q2)) {
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
    static qnode *getMiddle(qnode *S) {
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
};
} // ADT
} // Core
} // ngx
#endif
