enum TimerMode {
    TM_ONCE = 1,
    TM_INTERVAL = 2,
};

class Timer : protected RBNode {
private:
    uint64_t Timestamp;
    TimerMode Mode;
    uint64_t Interval;
    void *pData;

    virtual int operator-(RBNode &R) {
        return Timestamp > static_cast<Timer &>(R).Timestamp ? 1 : -1;
    }

    virtual int operator-(uint64_t R) {
        return this->Timestamp > R ? 1 : -1;
    }

public:
    friend class TimerTree;

    explicit Timer() : Mode(TM_ONCE), Interval(0), Timestamp(0), pData(nullptr) {};

    explicit Timer(TimerMode Mode, uint64_t Interval, void *pData) : Mode(Mode), Interval(Interval), Timestamp(0),
                                                                     pData(pData) {};

    explicit Timer(Timer &T) : Mode(T.Mode), Interval(T.Interval), Timestamp(T.Timestamp), pData(T.pData) {};

    Timer &operator=(Timer &T) { return {T}; }

    Timer &operator=(Timer &&T) { return {T}; }

    inline bool isAttached() const { return getParent() != this; };

    inline bool isInterval() const { return Mode == TM_INTERVAL; }

    inline uint64_t getTimestamp() const { return Timestamp; }

    inline void *getData() const { return pData; }
};

class TimerTree : protected RBTree {
protected:
    RuntimeError attach(Timer &T);

    RuntimeError detach(Timer &T);

public:
    TimerTree() = default;

    ~TimerTree();

    RuntimeError setOnce(Timer &T, uint64_t Peroid, void *pData);

    RuntimeError setInterval(Timer &T, uint64_t Interval, void *pData);

    RuntimeError respawnInterval(Timer &T);

    RuntimeError preempt(Timer &T);

    RuntimeError stop(Timer &T);

    Timer *begin() const;

    Timer *next(Timer *) const;
};

