class Reusable : public CanReset {
public:
    Queue ReuseItem;

    Reusable() : ReuseItem() {};

    virtual void Reset() {};
};
