class Reusable {
public:
    ADT::Queue ReuseItem;

    Reusable() : ReuseItem() {};

    virtual void Reset() {};
};
