#ifndef LIBNGX_CORE_ADT_QUEUE
#define LIBNGX_CORE_ADT_QUEUE
namespace ngx {
namespace Core {
namespace ADT {
class qnode {
protected:
    qnode *Prev = this, *Next = this;

public:
    qnode();
    bool isEmpty() const;
    qnode *next() const;
    qnode *prev() const;
    void append(qnode *Node);
    void detach();
    static void queueSplit(qnode *Q, qnode *Q1, qnode *Q2);
    static void sort(qnode *S, int (*cmp)(qnode *q1, qnode *q2));
    static qnode *getMiddle(qnode *S);
};
} // ADT
} // Core
} // ngx
#endif
