enum TimerMode {
    TM_CLOSED = 0,
    TM_ONCE,
    TM_INTERVAL,
};

class Timer : protected RBNode {
private:
    uint64_t Timestamp = 0;
    TimerMode Mode;
    uint64_t Interval;
    Job J;

    virtual int operator-(RBNode &R) {
        return Timestamp > ((Timer &) R).Timestamp ? 1 : -1;
    }

    virtual int operator-(uint64_t R) {
        return this->Timestamp > R ? 1 : -1;
    }

public:
    friend class TimerTree;
    explicit Timer() :Timestamp(0), Mode(TM_CLOSED), Interval(-1), J(){};
    explicit Timer( uint64_t    Timestamp,
                    TimerMode   Mode,
                    uint64_t    Interval,
                    Job         J) :Timestamp(Timestamp), Mode(Mode), Interval(Interval), J(J){};

    Timer(Timer &T) :Timestamp(T.Timestamp), Mode(T.Mode), Interval(T.Interval), J(T.J) {};
    Timer& operator = (Timer &T) { return {T}; }
    Timer& operator = (Timer &&T) { return {T}; }

    inline bool isAttached() const { return getParent()!=this; };
    inline uint64_t getTimestamp() const { return Timestamp; }
};

class TimerTree : protected RBTree {
protected:
    RuntimeError attach(Timer &T);
    RuntimeError detach(Timer &T);
public:
    TimerTree() = default;
    ~TimerTree();

    RuntimeError setOnce(Timer &T, uint64_t Peroid, Job &J);
    RuntimeError setInterval(Timer &T, uint64_t Interval, Job &J);
    RuntimeError preemptTimer(Timer &T);
    RuntimeError resetTimer(Timer &T);

    Timer *begin() const;
    Timer *next(Timer*) const;
};

