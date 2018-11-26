class Recyclable : public CanReset {
public:
    Queue RecycleItem;
    Recyclable() : RecycleItem() {};

    virtual void Reset() {};
};
