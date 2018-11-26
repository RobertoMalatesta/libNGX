class Queue {

protected:
    Queue *Prev = this, *Next = this;

public:
    Queue();

    bool IsEmpty();

    Queue *GetNext();

    Queue *GetPrev();

    void Attach(Queue *Node);

    void Detach();

    void Append(Queue *Node);

    static void QueueSplit(Queue *Q, Queue *Q1, Queue *Q2);

    static void Sort(Queue *S, int (*cmp)(Queue *q1, Queue *q2));

    static Queue *GetMiddle(Queue *S);
};
