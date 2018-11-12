class Recyclable : public CanReset, public Queue {
public:
    Recyclable() : Queue() {};

    virtual void Reset() {};
};
