namespace ngx::Core {

    class Queue {

        private:
            Queue   *Prev = this,  *Next = this, *Sentinel;
            void InsertToHead();
            void InsertToTail();
        public:
            Queue();
            Queue(Queue *Sentinel, bool IsInsertToHead);

            bool IsEmpty();
            Queue *GetHead();
            Queue *GetNext();
            Queue *GetPrev();
            Queue *GetLast();
            Queue *GetSentinel();
            Queue * GetMiddle();

            void Detach();
            void Attach();

            void Append(Queue *Node);

            void QueueSplit(Queue *Q1, Queue *Node);
            void Sort( int (*cmp) (Queue *q1, Queue*q2));
    };

}